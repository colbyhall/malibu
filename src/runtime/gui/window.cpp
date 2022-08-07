#include "window.hpp"
#include "command_list.hpp"

#include "gui.hpp"

namespace gui {
	void Window::set_widget_internal(SharedRef<Widget>&& widget) {
		m_widget = core::forward<SharedRef<Widget>>(widget);

		const auto client_size = client().size().cast<f32>();

		Layout layout = {};
		layout.local_size = client_size;
		layout.local_to_absolute = Mat3f32::identity();

		auto& the_widget = m_widget.as_mut().unwrap();
		the_widget->on_layout(layout);
	}

	void Window::paint() {
		if (m_widget) {
			auto& widget = m_widget.as_ref().unwrap();

			auto canvas = draw2d::Canvas::make();
			widget->on_paint(canvas);

			auto vertices = gpu::Buffer::make(
				gpu::BufferUsage::Vertex,
				gpu::BufferKind::Upload,
				canvas.vertices().len(),
				sizeof(draw2d::Canvas::Vertex)
			);
			vertices.write([&canvas](Slice<u8> slice) {
				core::mem::copy(slice.ptr(), canvas.vertices().ptr(), slice.len());
			});

			auto command_list = gpu::GraphicsCommandList::make();
			command_list.record([&](gpu::GraphicsCommandRecorder& gcr) {
				auto& back_buffer = m_swapchain.back_buffer();
				gcr.texture_barrier(
					back_buffer,
					gpu::Layout::Present,
					gpu::Layout::ColorAttachment
				);

				gcr.render_pass(
					back_buffer,
					NONE,
					[&](gpu::RenderPassRecorder& rpr) {
						const auto client = this->client().size().cast<f32>();

						const auto projection = Mat4f32::orthographic(
							client.width,
							client.height,
							0.01f,
							1000000.f
						);

						const auto view = Mat4f32::transform(
							{ -client.width / 2.f, -client.height / 2.f, 0.f },
							Quatf32::identity(),
							1.f
						);

						const auto local_to_projection = projection * view;


						rpr.clear_color(LinearColor::BLACK);
						rpr.set_pipeline(Manager::the().draw_pipeline);
						rpr.push_constants(&local_to_projection);
						rpr.set_vertices(vertices);
						rpr.draw(vertices.len());
					}
				);

				gcr.texture_barrier(
					back_buffer,
					gpu::Layout::ColorAttachment,
					gpu::Layout::Present
				);
			});

			command_list.submit();
			m_swapchain.present();
		}
	}
}