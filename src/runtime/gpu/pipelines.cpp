#include "pipelines.hpp"
#include "context.hpp"

#include "d3d12/d3d12_pipelines.hpp"

namespace gpu {
	VertexShader VertexShader::make(Array<u8>&& binary, Array<InputParameter>&& input_parameters) {
		auto& context = Context::the();

		Option<SharedRef<VertexShaderInterface>> interface;
		switch (context.backend()) {
			case Backend::D3D12:
				interface = SharedRef<VertexShaderInterface>::make(D3D12VertexShader(core::forward<Array<u8>>(binary), core::forward<Array<InputParameter>>(input_parameters)));
				break;
		}

		return VertexShader { interface.unwrap() };
	}

	PixelShader PixelShader::make(Array<u8>&& binary) {
		auto& context = Context::the();

		Option<SharedRef<PixelShaderInterface>> interface;
		switch (context.backend()) {
		case Backend::D3D12:
			interface = SharedRef<PixelShaderInterface>::make(D3D12PixelShader(core::forward<Array<u8>>(binary)));
			break;
		}

		return PixelShader { interface.unwrap() };
	}

	GraphicsPipeline GraphicsPipeline::make(GraphicsPipelineConfig&& config) {
		auto& context = Context::the();

		Option<SharedRef<GraphicsPipelineInterface>> interface;
		switch (context.backend()) {
			case Backend::D3D12:
				interface = SharedRef<GraphicsPipelineInterface>::make(D3D12GraphicsPipeline(core::move(config)));
				break;
		}

		return GraphicsPipeline { interface.unwrap() };
	}
}