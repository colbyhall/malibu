#include "gui.hpp"
#include "shape.hpp"
#include "math/aabb2.hpp"
#include "fs.hpp"
#include "dxc.hpp"

using namespace core;

namespace gui {
	static Option<Manager> g_manager = NONE;

	void init() {
		String source = fs::read_to_string("src/shaders/gui.hlsl").unwrap();

		dxc::Input vertex_input = {
			source,
			"vs_main",
			gpu::ShaderType::Vertex
		};
		auto vertex_output = dxc::compile(vertex_input).unwrap();
		auto vertex_shader = gpu::VertexShader::make(
			core::move(vertex_output.binary),
			core::move(vertex_output.input_parameters)
		);

		dxc::Input pixel_input = {
			source,
			"ps_main",
			gpu::ShaderType::Pixel
		};
		auto pixel_output = dxc::compile(pixel_input).unwrap();
		auto pixel_shader = gpu::PixelShader::make(core::move(pixel_output.binary));

		gpu::GraphicsPipelineConfig config = {
			core::move(vertex_shader),
			core::move(pixel_shader)
		};

		config.color_attachments.push(gpu::Format::RGBA_U8);

		config.blend_enabled = true;
		config.src_color_blend_factor = gpu::BlendFactor::SrcAlpha;
		config.dst_color_blend_factor = gpu::BlendFactor::OneMinusSrcAlpha;

		auto pipeline = gpu::GraphicsPipeline::make(core::move(config));

		g_manager = Manager{
			core::move(pipeline),
			draw::Font::import("assets/consola.ttf").unwrap()
		};
	}

	Manager const& Manager::the() {
		return g_manager.as_ref().unwrap();
	}
}