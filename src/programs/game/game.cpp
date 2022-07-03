#include "core.hpp"
#include "window.hpp"
#include "fs.hpp"
#include "time.hpp"
#include "math.hpp"

#include "containers/function.hpp"

#include "async/job.hpp"

#include "fbx.hpp"

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
		.visibility = WindowVisibility::Hidden,
	}).unwrap();

	async::schedule([]() {
		OutputDebugStringA("Hello World\n");
	});

	Vec3f32 xyz = 0.f;
	const auto f = xyz.dot(xyz);

    auto& context = gpu::Context::the();
	const auto registered = context.register_window(window);
	VERIFY(registered);

	auto mesh = fbx::load_mesh("assets/humanoid.fbx").unwrap();

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
	config.vertex_primitives.push(gpu::Primitive::Vec3f32);

	auto pipeline = gpu::GraphicsPipeline::make(core::move(config));

	auto vertices = gpu::Buffer::make(
		gpu::BufferUsage::Vertex,
		gpu::BufferKind::Upload,
		mesh.vertices.len(), sizeof(fbx::Vertex)
	);
	vertices.write([&mesh](Slice<u8> slice){
		mem::copy(slice.ptr(), mesh.vertices.ptr(), slice.len());
	});

	auto indices = gpu::Buffer::make(
		gpu::BufferUsage::Index,
		gpu::BufferKind::Upload,
		mesh.indices.len(), sizeof(u32)
	);
	indices.write([&mesh](Slice<u8> slice) {
		mem::copy(slice.ptr(), mesh.indices.ptr(), slice.len());
	});

	auto command_list = gpu::GraphicsCommandList::make();

	bool first_show = true;
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
				const auto client = window.client_size().cast<f32>();
                const auto projection = Mat4f32::perspective(
					90.f,
					client.width / client.height, 
					0.1f, 
					1000.f
				); 
				const auto view = Mat4f32::transform(
					{ 0, -100, -100 }, 
					Quatf32::identity(),
					1.f
				);
				const auto local_to_projection = projection * view;
				rp.set_pipeline(pipeline);
                rp.push_constants(&local_to_projection);
				rp.set_vertices(vertices);
				rp.set_indices(indices);
				rp.draw_index(indices.len());
			});
			recorder.texture_barrier(back_buffer, gpu::Layout::ColorAttachment, gpu::Layout::Present);
		});

		command_list.submit();
		context.present();
		if (first_show) {
			first_show = false;
			window.set_visibility(WindowVisibility::Visible);
		}
	}

	return 0;
}