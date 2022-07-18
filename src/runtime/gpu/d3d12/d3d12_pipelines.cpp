#include "d3d12_pipelines.hpp"
#include "d3d12_context.hpp"
#include "d3d12_resources.hpp"

#include "library.hpp"
using namespace core::library;

struct Dx12ShaderCompiler {
	Library d3dcompiler;
	pD3DCompile compile;

	static Dx12ShaderCompiler make() {
		Dx12ShaderCompiler ret;
		ret.d3dcompiler = Library::open("d3dcompiler_47.dll").unwrap();
		ret.compile = (pD3DCompile)ret.d3dcompiler.find("D3DCompile");
		return ret;
	}
};

Result<Array<u8>, String> gpu::compile_hlsl(StringView source, gpu::ShaderType type) {
	static Dx12ShaderCompiler compiler = Dx12ShaderCompiler::make();

	const char* entry_point = nullptr;
	switch (type) {
		case gpu::ShaderType::Vertex:
			entry_point = "vs_main";
			break;
		case gpu::ShaderType::Pixel:
			entry_point = "ps_main";
			break;
		default:
			INVALID_CODE_PATH;
			break;
	}

	const char* target = nullptr;
	switch (type) {
		case gpu::ShaderType::Vertex:
			target = "vs_5_0";
			break;
		case gpu::ShaderType::Pixel:
			target = "ps_5_0";
			break;
		default:
			INVALID_CODE_PATH;
			break;
	}

	ComPtr<ID3DBlob> binary;
	ComPtr<ID3DBlob> error;
	HRESULT result = (compiler.compile)(
		source.ptr(),
		source.len(),
		nullptr,
		nullptr,
		nullptr,
		entry_point,
		target,
		0,
		0,
		&binary,
		&error
	);

	if (result != S_OK) {
		String error_string;
		error_string.reserve(error->GetBufferSize());
		error_string.set_len(error->GetBufferSize());
		core::mem::copy(error_string.ptr(), error->GetBufferPointer(), error_string.len());
		error_string.ptr()[error_string.len()] = 0;
		return error_string;
	}

	Array<u8> binary_buffer;
	binary_buffer.reserve(binary->GetBufferSize());
	binary_buffer.set_len(binary->GetBufferSize());
	core::mem::copy(binary_buffer.ptr(), binary->GetBufferPointer(), binary_buffer.len());
	
	return binary_buffer;
}

D3D12GraphicsPipeline::D3D12GraphicsPipeline(gpu::GraphicsPipelineConfig&& config)
: m_config(core::forward<gpu::GraphicsPipelineConfig>(config)) {
	auto& context = gpu::Context::the().interface<D3D12Context>();

	VERIFY(m_config.vertex_shader.is_set() && m_config.pixel_shader.is_set());

	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = context.root_signature.Get();

	// Vertex shader
	{
		auto& shader = m_config.vertex_shader.as_ref().unwrap();
		VERIFY(shader.type() == gpu::ShaderType::Vertex);
		auto binary = shader.binary();
		desc.VS.pShaderBytecode = binary.ptr();
		desc.VS.BytecodeLength = binary.len();
	}

	// Pixel shader
	{
		auto& shader = m_config.pixel_shader.as_ref().unwrap();
		VERIFY(shader.type() == gpu::ShaderType::Pixel);
		auto binary = shader.binary();
		desc.PS.pShaderBytecode = binary.ptr();
		desc.PS.BytecodeLength = binary.len();
	}

	// BlendState
	{
		D3D12_BLEND_DESC blend = {};
		blend.AlphaToCoverageEnable = FALSE;
		blend.IndependentBlendEnable = FALSE;

		const D3D12_RENDER_TARGET_BLEND_DESC def = {
			FALSE,
			FALSE,
			
			D3D12_BLEND_ONE, 
			D3D12_BLEND_ZERO, 
			D3D12_BLEND_OP_ADD,
			
			D3D12_BLEND_ONE, 
			D3D12_BLEND_ZERO, 
			D3D12_BLEND_OP_ADD,
			
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		};
		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
			blend.RenderTarget[i] = def;
		}

		desc.BlendState = blend;
	}

	// RasterizerState
	{
		// TODO: Finish this
		D3D12_RASTERIZER_DESC rasterizer_state = {};
		rasterizer_state.FillMode = D3D12_FILL_MODE_SOLID;
		switch (m_config.cull_mode) {
		case gpu::CullMode::None:
			rasterizer_state.CullMode = D3D12_CULL_MODE_NONE;
			break;
		case gpu::CullMode::Front:
			rasterizer_state.CullMode = D3D12_CULL_MODE_FRONT;
			break;
		case gpu::CullMode::Back:
			rasterizer_state.CullMode = D3D12_CULL_MODE_BACK;
			break;
		}
		rasterizer_state.FrontCounterClockwise = FALSE;
		rasterizer_state.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterizer_state.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizer_state.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizer_state.DepthClipEnable = TRUE;
		rasterizer_state.MultisampleEnable = FALSE;
		rasterizer_state.AntialiasedLineEnable = FALSE;
		rasterizer_state.ForcedSampleCount = 0;
		rasterizer_state.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		desc.RasterizerState = rasterizer_state;
	}

	// DepthStencilState
	{
		desc.DepthStencilState.DepthEnable = TRUE;
		desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		desc.DepthStencilState.StencilEnable = FALSE;
	}

	// TEMP FIX
	const char* SemanticNames[] = {
		"POSITION",
		"COLOR"
	};

	// InputLayout
	Array<D3D12_INPUT_ELEMENT_DESC> input_layout;
	input_layout.reserve(m_config.vertex_primitives.len());
	usize offset = 0;
	for (int i = 0; i < m_config.vertex_primitives.len(); ++i) {
		D3D12_INPUT_ELEMENT_DESC input = {};
		// input.SemanticIndex = i;
		input.SemanticName = SemanticNames[i];

		usize size_in_bytes = 0;
		switch (m_config.vertex_primitives[i]) {
			case gpu::Primitive::Uint32:
				input.Format = DXGI_FORMAT_R32_UINT;
				size_in_bytes = 4;
				break;
			case gpu::Primitive::Int32:
				input.Format = DXGI_FORMAT_R32_SINT;
				size_in_bytes = 4;
				break;
			case gpu::Primitive::Float32:
				input.Format = DXGI_FORMAT_R32_FLOAT;
				size_in_bytes = 4;
				break;
			case gpu::Primitive::Vec2f32:
				input.Format = DXGI_FORMAT_R32G32_FLOAT;
				size_in_bytes = 8;
				break;
			case gpu::Primitive::Vec3f32:
				input.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				size_in_bytes = 12;
				break;
			case gpu::Primitive::Vec4f32:
				input.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				size_in_bytes = 16;
				break;
			case gpu::Primitive::Mat4f32:
				PANIC("");
				break;
		}
		input.AlignedByteOffset = (UINT)offset;
		input.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

		input_layout.push(input);
		offset += size_in_bytes;
	}
	desc.InputLayout.pInputElementDescs = input_layout.ptr();
	desc.InputLayout.NumElements = (UINT)input_layout.len();

	desc.NumRenderTargets = (UINT)m_config.color_attachments.len();
	for (int i = 0; i < m_config.color_attachments.len(); ++i) {
		desc.RTVFormats[i] = format_to_dxgi(m_config.color_attachments[i]);
	}

	if (m_config.depth_attachment) {
		desc.DSVFormat = format_to_dxgi(m_config.depth_attachment.as_ref().unwrap());
	}

	desc.SampleMask = UINT_MAX;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.SampleDesc.Count = 1;

	throw_if_failed(context.device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_pipeline)));
}