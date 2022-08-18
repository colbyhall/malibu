#pragma once

#include "pipelines.hpp"
#include "font.hpp"

namespace gui {
	struct Manager {
		gpu::GraphicsPipeline draw_pipeline;
		draw2d::Font consola;

		static Manager const& the();
	};

	void init();
}