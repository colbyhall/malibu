#include "basic_shapes.hpp"
#include "canvas.hpp"

namespace draw2d {
	void Rect::set_texture(const gpu::Texture& texture, Vec2f32 uv0, Vec2f32 uv1) {
		m_texture = 1; // texture.bindless();
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

	void Text::triangulate(Canvas& canvas) const {
		const auto origin = m_bounds.top_left();
		const f32 size = 22.f;
		const f32 scale = m_size / size;

		auto position = origin;
		position.y -= size * scale * 2.f;

		for (auto codepoints = m_text.codepoints(); codepoints; ++codepoints) {
			auto c = *codepoints;

			switch (c) {
			case '\n': {
				position.x = origin.x;
				position.y -= size * scale;
			} break;
			case '\r': {
				position.x = origin.x;
			} break;
			case '\t': {
				auto& space_glyph = m_font.glyph(' ');
				position.x += space_glyph.advance * scale;
			} break;
			default:
				auto& glyph = m_font.glyph(c);

				Rect rect = Rect2f32::from_min_max(position, position + glyph.size * scale);
				rect.set_texture(m_font.atlas(), glyph.uv0, glyph.uv1);
				canvas.paint(rect);

				position.x += glyph.advance * scale;
				break;
			}
		};
	}
}