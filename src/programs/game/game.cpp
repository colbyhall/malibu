#include "core.hpp"
#include "window.hpp"
#include "fs.hpp"
#include "time.hpp"
#include "math.hpp"

#include "containers/function.hpp"

#include "async/job.hpp"

using namespace core;
using namespace core::window;
using namespace core::time;

#include "gpu.hpp"

#include <cstdio>

// #define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define WIN32_MEAN_AND_LEAN
#include <windows.h>

static bool g_running = true;

void window_callback(WindowHandle window, const WindowEvent& event) {
	if (event.type == WindowEventType::ExitRequested || event.type == WindowEventType::Closed) {
		g_running = false;
		async::shutdown();
	}
}

struct Vertex {
	Vec3f32 position;
	LinearColor color;
};

void whatever(FunctionRef<u32()> f) {
	const auto result = f();
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
) {
	auto window = Window::make({
		.size = { 1280, 720 },
		.title = "Hello World",
		.callback = window_callback,
		.visibility = WindowVisibility::Visible,
	}).unwrap();

	async::schedule([]() {
		OutputDebugStringA("hello world1\n");
		async::schedule([]() {
			OutputDebugStringA("hello world2\n");
		});
	});
	async::schedule([]() {
		OutputDebugStringA("hello world3\n");
	});
	async::schedule([]() {
		OutputDebugStringA("hello world4\n");
	});

    auto& context = gpu::Context::the();
	const auto registered = context.register_window(window);
	VERIFY(registered);

	String shader = fs::read_to_string("src/shaders/triangle.hlsl").unwrap();
	auto vertex_binary = gpu::compile_hlsl(shader, gpu::ShaderType::Vertex).unwrap();
	auto vertex_shader = gpu::Shader::make(core::move(vertex_binary), gpu::ShaderType::Vertex);

	auto pixel_binary = gpu::compile_hlsl(shader, gpu::ShaderType::Pixel).unwrap();
	auto pixel_shader = gpu::Shader::make(core::move(pixel_binary), gpu::ShaderType::Pixel);

	gpu::GraphicsPipelineConfig config = {};
	config.color_attachments.push(gpu::Format::RGBA_U8);
	config.vertex_shader = core::move(vertex_shader);
	config.pixel_shader = core::move(pixel_shader);

	config.vertex_primitives.push(gpu::Primitive::Vec3f32);
	config.vertex_primitives.push(gpu::Primitive::Vec4f32);

	auto pipeline = gpu::GraphicsPipeline::make(core::move(config));

	auto triangle = gpu::Buffer::make(
		gpu::BufferUsage::Vertex,
		gpu::BufferKind::Upload,
		6, sizeof(Vertex)
	);
	triangle.write([](Slice<u8> slice){
		Vertex vertices[] = {
            { { -0.5f, -0.5f, 0.f }, LinearColor::WHITE },
			{ { -0.5f,  0.5f, 0.f }, LinearColor::WHITE },
			{ {  0.5f,  0.5f, 0.f }, LinearColor::WHITE },

            { { -0.5f, -0.5f, 0.f }, LinearColor::WHITE },
            { {  0.5f,  0.5f, 0.f }, LinearColor::WHITE },
            { {  0.5f, -0.5f, 0.f }, LinearColor::WHITE },
		};
		core::mem::copy(slice.ptr(), vertices, slice.len());
	});

	auto command_list = gpu::GraphicsCommandList::make();

	auto last_frame = Instant::now();
	while (g_running) {
		const auto now = Instant::now();
		const auto delta = now.duration_since(last_frame);
		last_frame = now;

		Window::pump_events();

		command_list.record([&](gpu::GraphicsCommandRecorder& recorder){
			auto& back_buffer = context.back_buffer();
			recorder.texture_barrier(back_buffer, gpu::Layout::Present, gpu::Layout::ColorAttachment);
			recorder.render_pass(back_buffer, [&](gpu::RenderPassRecorder& rp) {
				const auto client = window.client_size().cast<f32>() / 256;
                const auto identity = Mat4f32::orthographic(client.width, client.height, 0.1f, 100.f);
				rp.set_pipeline(pipeline);
                rp.push_constants(&identity);
				rp.set_vertices(triangle);
				rp.draw(triangle.len());
			});
			recorder.texture_barrier(back_buffer, gpu::Layout::ColorAttachment, gpu::Layout::Present);
		});

		command_list.submit();
		context.present();
	}

	return 0;
}