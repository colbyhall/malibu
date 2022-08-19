#include "basic_shapes.hpp"
#include "canvas.hpp"
#include "resources.hpp"

namespace draw {
	void Rect::set_texture(const gpu::Texture& texture, Vec2f32 uv0, Vec2f32 uv1) {
		m_texture = texture.bindless();
		m_uv0 = uv0;
		m_uv1 = uv1;
	}

	void Rect::triangulate(Canvas& canvas) const {
		canvas.add_vertex(m_bounds.bottom_left(), m_uv0, m_texture);
		canvas.add_vertex(m_bounds.top_left(), { m_uv0.x, m_uv1.y }, m_texture);
		canvas.add_vertex(m_bounds.top_right(), m_uv1, m_texture);

		canvas.add_vertex(m_bounds.bottom_left(), m_uv0, m_texture);
		canvas.add_vertex(m_bounds.top_right(), m_uv1, m_texture);
		canvas.add_vertex(m_bounds.bottom_right(), { m_uv1.x, m_uv0.y }, m_texture);
	}
}