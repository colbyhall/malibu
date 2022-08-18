#include "image_element.hpp"
#include "canvas.hpp"
#include "basic_shapes.hpp"

namespace gui {
	void ImageElement::on_paint(draw::Canvas& canvas) const {
		auto& layout = m_layout.as_ref().unwrap();

		const auto local_position = Vec3f32{ layout.local_position, 1.f };
		const auto local_size = Vec3f32{ layout.local_size, 0.f };

		const auto absolute_position = (layout.local_to_absolute * local_position).xy;
		const auto absolute_size = (layout.local_to_absolute * local_size).xy;

		const auto min = absolute_position;
		const auto max = min + absolute_size;

		canvas.set_color(m_color);

		auto rect = draw::Rect(AABB2f32::from_min_max(min, max));
		rect.set_texture(m_texture, m_uv0, m_uv1);

		canvas.paint(rect);
	}
}