#include "pipelines.hpp"
#include "context.hpp"

#include "dx12/dx12_pipelines.hpp"

namespace gpu {
	Shader Shader::create(StringView source) {
		auto& context = Context::the();

		Option<SharedRef<ShaderInterface>> interface;
		switch (context.backend()) {
			case Backend::Dx12:
				interface = SharedRef<ShaderInterface>(Dx12Shader(source));
				break;
		}

		return Shader { interface.unwrap() };
	}

	GraphicsPipeline GraphicsPipeline::create(GraphicsPipelineConfig&& config) {
		auto& context = Context::the();

		Option<SharedRef<GraphicsPipelineInterface>> interface;
		switch (context.backend()) {
			case Backend::Dx12:
				interface = SharedRef<GraphicsPipelineInterface>(Dx12GraphicsPipeline(core::move(config)));
				break;
		}

		return GraphicsPipeline { interface.unwrap() };
	}
}