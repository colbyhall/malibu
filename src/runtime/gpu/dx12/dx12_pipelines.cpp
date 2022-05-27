#include "dx12_pipelines.hpp"
#include "dx12_context.hpp"

Dx12Shader::Dx12Shader(StringView source, gpu::ShaderType type) {

}

Dx12GraphicsPipeline::Dx12GraphicsPipeline(gpu::GraphicsPipelineConfig&& config) 
	: m_config(core::move(config)) {
	auto& context = gpu::Context::the().interface<Dx12Context>();

/*	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	desc.pRootSignature = m_rootSignature.Get();
	desc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	desc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	desc.DepthStencilState.DepthEnable = FALSE;
	desc.DepthStencilState.StencilEnable = FALSE;
	desc.SampleMask = UINT_MAX;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;*/


}