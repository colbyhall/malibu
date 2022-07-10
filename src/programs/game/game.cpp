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

struct Camera {
	bool capture_mouse = false;

	f32 pitch;
	f32 yaw;

	f32 speed = 5.f;
	f32 fov = 90.f;

	Vec3f32 position;
};

static Camera g_camera = {};

struct Input {
	bool keys_pressed[256];
	bool last_keys_pressed[256];

	inline bool was_key_pressed(int key) const {
		return keys_pressed[key] && !last_keys_pressed[key];
	}
};

static Input g_input = {};

static Option<Window> g_window;

void window_callback(WindowHandle window, const WindowEvent& event) {
	switch (event.type) {
	case WindowEventType::Closed:
	case WindowEventType::ExitRequested:
		g_running = false;
		async::shutdown_job_system();
		break;
	case WindowEventType::MouseMoved:
		if (g_camera.capture_mouse) {
			const auto sensitivity = 0.3f;
			g_camera.pitch -= (f32)event.mouse_moved.delta.y * sensitivity;
			g_camera.yaw += (f32)event.mouse_moved.delta.x * sensitivity;
		}
		break;
	case WindowEventType::Key:
		g_input.keys_pressed[event.key.vk] = event.key.pressed;
		break;
	case WindowEventType::MouseButton:
		if (event.mouse_button.button == window::MouseButton::Right)  {
			g_camera.capture_mouse = event.mouse_button.pressed;
			auto& win = g_window.as_ref().unwrap();
			win.set_cursor_lock(g_camera.capture_mouse);
			win.set_cursor_visbility(!g_camera.capture_mouse);
		}
		break;
	case WindowEventType::MouseWheel:
		g_camera.speed += event.mouse_wheel.delta;
		const auto min = 0.1f;
		if (g_camera.speed < min) g_camera.speed = min;
		break;
	};
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
) {
	async::init_job_system();

#if 0
	auto result = async::schedule2([]() -> u32 {
		return 420;
	});

	while (!result.is_ready()) {
		OutputDebugStringA("Waiting\n");
	}
#endif

	g_window = Window::make({
		{ 1920, 1080 },
		"Malibu",
		window_callback,
		WindowVisibility::Hidden,
	}).unwrap();

    auto& context = gpu::Context::the();
	const auto registered = context.register_window(g_window.as_ref().unwrap());
	VERIFY(registered);

	auto scene = fbx::load_fbx_scene("assets/human_head.fbx").unwrap();
	auto& mesh = scene.meshes[0];

	String shader = fs::read_to_string("src/shaders/triangle.hlsl").unwrap();
	auto vertex_binary = gpu::compile_hlsl(shader, gpu::ShaderType::Vertex).unwrap();
	auto vertex_shader = gpu::Shader::make(core::move(vertex_binary), gpu::ShaderType::Vertex);

	auto pixel_binary = gpu::compile_hlsl(shader, gpu::ShaderType::Pixel).unwrap();
	auto pixel_shader = gpu::Shader::make(core::move(pixel_binary), gpu::ShaderType::Pixel);

	gpu::GraphicsPipelineConfig config = {};
	config.color_attachments.push(gpu::Format::RGBA_U8);
	config.depth_attachment = gpu::Format::Depth24_Stencil8;
	config.vertex_shader = core::move(vertex_shader);
	config.pixel_shader = core::move(pixel_shader);

	config.vertex_primitives.push(gpu::Primitive::Vec3f32);
	config.vertex_primitives.push(gpu::Primitive::Vec3f32);

	auto pipeline = gpu::GraphicsPipeline::make(core::move(config));

	auto vertices = gpu::Buffer::make(
		gpu::BufferUsage::Vertex,
		gpu::BufferKind::Upload,
		mesh.vertices.len(), 
		sizeof(fbx::Vertex)
	);
	vertices.write([&mesh](Slice<u8> slice){
		mem::copy(slice.ptr(), mesh.vertices.ptr(), slice.len());
	});

	auto indices = gpu::Buffer::make(
		gpu::BufferUsage::Index,
		gpu::BufferKind::Upload,
		mesh.indices.len(), 
		sizeof(u32)
	);
	indices.write([&mesh](Slice<u8> slice) {
		mem::copy(slice.ptr(), mesh.indices.ptr(), slice.len());
	});

	auto command_list = gpu::GraphicsCommandList::make();

	auto depth_buffer = gpu::Texture::make(
		gpu::TextureUsage::DepthAttachment, 
		gpu::Format::Depth24_Stencil8,
		{ 1920, 1080, 1 }
	);

	bool first_show = true;
	auto last_frame = Instant::now();
	f32 time = 0.f;
	while (g_running) {
		const auto now = Instant::now();
		const auto delta = now.duration_since(last_frame);
		time += delta.as_secs_f32();
		last_frame = now;

		const auto dt = delta.as_secs_f32();

		mem::copy(g_input.last_keys_pressed, g_input.keys_pressed, 256);
		Window::pump_events();

		const auto view_rotation = Quatf32::from_euler(
			g_camera.pitch,
			g_camera.yaw,
			0.f
		);

		const auto forward = view_rotation.rotate_vector(Vec3f32::forward());
		const auto right = view_rotation.rotate_vector(Vec3f32::right());
		const auto up = Vec3f32::up();

		const auto speed = g_camera.speed;
		g_camera.position += forward * speed * (f32)g_input.keys_pressed['W'] * dt;
		g_camera.position -= forward * speed * (f32)g_input.keys_pressed['S'] * dt;

		g_camera.position += right * speed * (f32)g_input.keys_pressed['D'] * dt;
		g_camera.position -= right * speed * (f32)g_input.keys_pressed['A'] * dt;

		g_camera.position += up * speed * (f32)g_input.keys_pressed[' '] * dt;
		g_camera.position -= up * speed * (f32)g_input.keys_pressed[VK_CONTROL] * dt;

		const auto fov_speed = 5.f;
		g_camera.fov += (f32)g_input.keys_pressed['E'] * dt * fov_speed;
		g_camera.fov -= (f32)g_input.keys_pressed['Q'] * dt * fov_speed;

		const auto fov = g_camera.fov;

		command_list.record([&](gpu::GraphicsCommandRecorder& recorder) {
			auto& back_buffer = context.back_buffer();
			recorder.texture_barrier(
				back_buffer, 
				gpu::Layout::Present, 
				gpu::Layout::ColorAttachment
			);

			recorder.render_pass(
				back_buffer, 
				depth_buffer, 
				[&](gpu::RenderPassRecorder& rp) {
					const auto client = g_window.as_ref().unwrap().client_size().cast<f32>();
					const auto projection = Mat4f32::perspective(
						fov,
						client.width / client.height, 
						0.01f, 
						1000.f
					); 

					const auto axis_adjustment = Mat4f32::from_columns(
						{ 0.0, 0.0, -1.0, 0.0 },
						{ 1.0, 0.0, 0.0, 0.0 },
						{ 0.0, 1.0, 0.0, 0.0 },
						{ 0.0, 0.0, 0.0, 1.0 }
					);

					const auto view = Mat4f32::transform(
						g_camera.position, 
						view_rotation,
						1.f
					).inverse().unwrap();
					const auto local_to_projection = projection * axis_adjustment * view;

					rp.set_pipeline(pipeline);
					rp.push_constants(&local_to_projection);
					rp.set_vertices(vertices);
					rp.set_indices(indices);
					rp.draw_index(indices.len());
				}
			);

			recorder.texture_barrier(
				back_buffer, 
				gpu::Layout::ColorAttachment, 
				gpu::Layout::Present
			);
		});

		command_list.submit();
		context.present();
		if (first_show) {
			first_show = false;
			g_window.as_ref().unwrap().set_visibility(WindowVisibility::Visible);
		}
	}

	return 0;
}