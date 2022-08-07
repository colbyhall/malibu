#pragma once

#include "pipelines.hpp"

namespace gui {
	struct Manager {
		gpu::GraphicsPipeline draw_pipeline;

		static Manager const& the();
	};

	void init();
}