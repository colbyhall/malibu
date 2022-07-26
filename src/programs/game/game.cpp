﻿#include "core.hpp"
#include "window.hpp"
#include "fs.hpp"
#include "time.hpp"
#include "math.hpp"

#include "containers/function.hpp"

#include "async/job.hpp"
#include "asset.hpp"

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
		async::job::shutdown();
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

class Canvas {
public:
	static Canvas make() { return Canvas{}; }

	Canvas& line(Vec3f32 a, Vec3f32 b, f32 width, LinearColor color) {
		const auto a_to_b = b - a;
		if (a_to_b.len_sq() <= core::math::NEAR_ZERO) return *this;

		auto forward = a_to_b.normalize().unwrap();
		auto up = Vec3f32::up();
		auto right = forward.cross(up);

		if (forward.dot(up) > 0.7f) {
			if (forward.dot(Vec3f32::right()) > 0.7f) {
				right = Vec3f32::forward();
				up = right.cross(forward);
			} else {
				right = Vec3f32::right();
				up = right.cross(forward);
			}
		}

		Vec3f32::orthonormal_basis(forward, up, right);

		const auto half_width = width * 0.5f;

		// Draw the horizontal plane
		{
			const auto bl = a - right * half_width;
			const auto br = a + right * half_width;
			const auto tl = b - right * half_width;
			const auto tr = b + right * half_width;

			m_vertices.push(fbx::Vertex{
				bl,
				{ color.r, color.g, color.b }
			});
			m_vertices.push(fbx::Vertex{
				tl,
				{ color.r, color.g, color.b }
			});
			m_vertices.push(fbx::Vertex{
				tr,
				{ color.r, color.g, color.b }
			});

			m_vertices.push(fbx::Vertex{
				bl,
				{ color.r, color.g, color.b }
			});
			m_vertices.push(fbx::Vertex{
				tr,
				{ color.r, color.g, color.b }
			});
			m_vertices.push(fbx::Vertex{
				br,
				{ color.r, color.g, color.b }
			});
		}

		// Draw the vertical plane
		{
			const auto bl = a - up * half_width;
			const auto br = a + up * half_width;
			const auto tl = b - up * half_width;
			const auto tr = b + up * half_width;

			m_vertices.push(fbx::Vertex{
				bl,
				{ color.r, color.g, color.b }
			});
			m_vertices.push(fbx::Vertex{
				tl,
				{ color.r, color.g, color.b }
			});
			m_vertices.push(fbx::Vertex{
				tr,
				{ color.r, color.g, color.b }
			});

			m_vertices.push(fbx::Vertex{
				bl,
				{ color.r, color.g, color.b }
			});
			m_vertices.push(fbx::Vertex{
				tr,
				{ color.r, color.g, color.b }
			});
			m_vertices.push(fbx::Vertex{
				br,
				{ color.r, color.g, color.b }
			});
		}

		return *this;
	}

	inline Slice<fbx::Vertex const> vertices() const { return m_vertices; }

private:
	inline Canvas() {}

	Array<fbx::Vertex> m_vertices;
};


int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
) {
	async::job::init();
	asset::init();

	g_window = Window::make({
		{ 1920, 1080 },
		"Malibu",
		window_callback,
		WindowVisibility::Hidden,
	}).unwrap();

	auto& context = gpu::Context::the();
	const auto registered = context.register_window(g_window.as_ref().unwrap());
	VERIFY(registered);

	auto scene0 = fbx::load_fbx_scene("assets/bee.fbx").unwrap();
	auto& bee = scene0.meshes[0];

	auto bee_vertices = gpu::Buffer::make(
		gpu::BufferUsage::Vertex,
		gpu::BufferKind::Upload,
		bee.vertices.len(),
		sizeof(fbx::Vertex)
	);
	bee_vertices.write([&bee](Slice<u8> slice) {
		mem::copy(slice.ptr(), bee.vertices.ptr(), slice.len());
	});

	auto bee_indices = gpu::Buffer::make(
		gpu::BufferUsage::Index,
		gpu::BufferKind::Upload,
		bee.indices.len(),
		sizeof(u32)
	);
	bee_indices.write([&bee](Slice<u8> slice) {
		mem::copy(slice.ptr(), bee.indices.ptr(), slice.len());
	});

	auto& bee_skeleton = scene0.skeletons[0];

	Canvas canvas = Canvas::make();
	for (auto& bone : bee_skeleton.bones) {
		char buffer[256];
		sprintf_s(buffer, "%s %f %f %f \n", bone.name.ptr(), bone.position.x, bone.position.y, bone.position.z);
		OutputDebugStringA(buffer);

		if (bone.parent != -1) {
			const auto& parent = bee_skeleton.bones[bone.parent];
			canvas.line(parent.position, bone.position, 10.f, { 1.f, 1.f, 0.f, 1.f });
		}
	}

	auto canvas_vertices = gpu::Buffer::make(
		gpu::BufferUsage::Vertex,
		gpu::BufferKind::Upload,
		canvas.vertices().len(),
		sizeof(fbx::Vertex)
	);
	canvas_vertices.write([&canvas](Slice<u8> slice) {
		mem::copy(slice.ptr(), canvas.vertices().ptr(), slice.len());
	});

	auto scene1 = fbx::load_fbx_scene("assets/box.fbx").unwrap();
	auto& box = scene1.meshes[0];

	auto box_vertices = gpu::Buffer::make(
		gpu::BufferUsage::Vertex,
		gpu::BufferKind::Upload,
		box.vertices.len(),
		sizeof(fbx::Vertex)
	);
	box_vertices.write([&box](Slice<u8> slice) {
		mem::copy(slice.ptr(), box.vertices.ptr(), slice.len());
	});

	auto box_indices = gpu::Buffer::make(
		gpu::BufferUsage::Index,
		gpu::BufferKind::Upload,
		box.indices.len(),
		sizeof(u32)
	);
	box_indices.write([&box](Slice<u8> slice) {
		mem::copy(slice.ptr(), box.indices.ptr(), slice.len());
	});


	gpu::GraphicsPipelineConfig static_lit_config = {};
	{
		String source = fs::read_to_string("src/shaders/static_lit.hlsl").unwrap();
		auto vertex_binary = gpu::compile_hlsl(source, gpu::ShaderType::Vertex).unwrap();
		auto vertex_shader = gpu::Shader::make(core::move(vertex_binary), gpu::ShaderType::Vertex);

		auto pixel_binary = gpu::compile_hlsl(source, gpu::ShaderType::Pixel).unwrap();
		auto pixel_shader = gpu::Shader::make(core::move(pixel_binary), gpu::ShaderType::Pixel);

		static_lit_config.color_attachments.push(gpu::Format::RGBA_U8);
		static_lit_config.depth_attachment = gpu::Format::Depth24_Stencil8;
		static_lit_config.vertex_shader = core::move(vertex_shader);
		static_lit_config.pixel_shader = core::move(pixel_shader);

		static_lit_config.cull_mode = gpu::CullMode::Back;

		static_lit_config.vertex_primitives.push(gpu::Primitive::Vec3f32);
		static_lit_config.vertex_primitives.push(gpu::Primitive::Vec3f32);
	}
	auto static_lit_pipeline = gpu::GraphicsPipeline::make(core::move(static_lit_config));

	gpu::GraphicsPipelineConfig debug_config = {};
	{
		String source = fs::read_to_string("src/shaders/debug3d.hlsl").unwrap();
		auto vertex_binary = gpu::compile_hlsl(source, gpu::ShaderType::Vertex).unwrap();
		auto vertex_shader = gpu::Shader::make(core::move(vertex_binary), gpu::ShaderType::Vertex);

		auto pixel_binary = gpu::compile_hlsl(source, gpu::ShaderType::Pixel).unwrap();
		auto pixel_shader = gpu::Shader::make(core::move(pixel_binary), gpu::ShaderType::Pixel);

		debug_config.color_attachments.push(gpu::Format::RGBA_U8);
		debug_config.depth_attachment = gpu::Format::Depth24_Stencil8;
		debug_config.vertex_shader = core::move(vertex_shader);
		debug_config.pixel_shader = core::move(pixel_shader);

		debug_config.vertex_primitives.push(gpu::Primitive::Vec3f32);
		debug_config.vertex_primitives.push(gpu::Primitive::Vec3f32);
	}
	auto debug3d_pipeline = gpu::GraphicsPipeline::make(core::move(debug_config));

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
						1000000.f
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

					rp.set_pipeline(static_lit_pipeline);

					rp.clear_color(LinearColor::BLACK);
					rp.clear_depth_stencil(1.f, 0);

					const auto local_to_projection = projection * axis_adjustment * view;

					// Draw the bee
					{
						rp.push_constants(&local_to_projection);
						rp.set_vertices(bee_vertices);
						rp.set_indices(bee_indices);
						rp.draw_index(bee_indices.len());
					}

					rp.clear_depth_stencil(1.f, 0);
					rp.set_pipeline(debug3d_pipeline);

					// Draw all the debug information
					{
						rp.set_vertices(canvas_vertices);
						rp.draw(canvas_vertices.len());
					}
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