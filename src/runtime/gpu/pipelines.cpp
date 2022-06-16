#include "pipelines.hpp"
#include "context.hpp"

#include "d3d12/d3d12_pipelines.hpp"

namespace gpu {
	Shader Shader::make(Array<u8>&& binary, ShaderType type) {
		auto& context = Context::the();

		Option<SharedRef<ShaderInterface>> interface;
		switch (context.backend()) {
			case Backend::D3D12:
				interface = SharedRef<ShaderInterface>::make(D3D12Shader(core::forward<Array<u8>>(binary), type));
				break;
		}

		return Shader { interface.unwrap() };
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