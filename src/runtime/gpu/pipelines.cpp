#include "pipelines.hpp"
#include "context.hpp"

#include "dx12/dx12_pipelines.hpp"

namespace gpu {
	Shader Shader::make(Array<u8>&& binary, ShaderType type) {
		auto& context = Context::the();

		Option<SharedRef<ShaderInterface>> interface;
		switch (context.backend()) {
			case Backend::Dx12:
				interface = SharedRef<ShaderInterface>::make(Dx12Shader(core::forward<Array<u8>>(binary), type));
				break;
		}

		return Shader { interface.unwrap() };
	}

	GraphicsPipeline GraphicsPipeline::make(GraphicsPipelineConfig&& config) {
		auto& context = Context::the();

		Option<SharedRef<GraphicsPipelineInterface>> interface;
		switch (context.backend()) {
			case Backend::Dx12:
				interface = SharedRef<GraphicsPipelineInterface>::make(Dx12GraphicsPipeline(core::move(config)));
				break;
		}

		return GraphicsPipeline { interface.unwrap() };
	}
}