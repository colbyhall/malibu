﻿#include "core/minimal.hpp"
#include "core/window.hpp"
#include "core/sync/mutex.hpp"
#include "core/fs.hpp"

using namespace core;
using namespace core::window;
using namespace core::time;
using namespace core::sync;

#include "gpu/minimal.hpp"

#include <cstdio>

static bool g_running = true;

void window_callback(WindowHandle window, const WindowEvent& event) {
	if (event.type == WindowEventType::ExitRequested || event.type == WindowEventType::Closed) {
		g_running = false;
	}
}

int main(int argc, char** argv) {
	auto window = Window::make({
		.size = { 1280, 720 },
		.title = "Hello World",
		.callback = window_callback,
		.visibility = WindowVisibility::Visible,
	}).unwrap();

	const gpu::Context& context = gpu::Context::the();

	const auto registered = context.register_window(window);
	VERIFY(registered);

	String shader = fs::read_to_string("foo.txt").unwrap();
	auto vertex_binary = gpu::compile_hlsl(shader, gpu::ShaderType::Vertex).unwrap();
	auto vertex_shader = gpu::Shader::make(core::move(vertex_binary), gpu::ShaderType::Vertex);

	auto pixel_binary = gpu::compile_hlsl(shader, gpu::ShaderType::Pixel).unwrap();
	auto pixel_shader = gpu::Shader::make(core::move(pixel_binary), gpu::ShaderType::Pixel);

	gpu::GraphicsPipelineConfig config = {};
	config.color_attachments.push(gpu::Format::RGBA_U8);
	config.vertex_shader = core::move(vertex_shader);
	config.pixel_shader = core::move(pixel_shader);
	
	config.vertex_primitives.push(gpu::Primitive::Vec4f32);
	config.vertex_primitives.push(gpu::Primitive::Vec4f32);

	auto pipeline = gpu::GraphicsPipeline::make(core::move(config));

	auto last_frame = Instant::now();
	while (g_running) {
		const auto now = Instant::now();
		const auto delta = now.duration_since(last_frame);
		last_frame = now;

		Window::pump_events();
	}

	return 0;
}