#pragma once

#include "core.hpp"

#include "math/vec4.hpp"
#include "math/color.hpp"

#include "shape.hpp"

namespace draw {
	class Canvas {
	public:
		struct Vertex {
			Vec3f32 position;
			LinearColor color;
			Vec4f32 scissor;
			Vec2f32 uv;
			u32 texture;
		};

		static Canvas make() { return Canvas{}; }

		inline Canvas& add_vertex(const Vec3f32& position, Vec2f32 uv = 0, u32 texture = 0) {
			Vec4f32 scissor = 0;
			const auto bottom_left = m_scissor.bottom_left();
			scissor.x = bottom_left.x;
			scissor.y = bottom_left.y;

			const auto top_right = m_scissor.top_right();
			scissor.z = top_right.x;
			scissor.w = top_right.y;

			m_vertices.push({
				position,
				m_color,
				scissor,
				uv,
				texture
			});
			return *this;
		}

		inline Canvas& paint(const Shape& t) {
			t.triangulate(*this);
			return *this;
		}

		inline Canvas& set_color(const LinearColor& color) { m_color = color; return *this; }
		inline Canvas& set_scissor(const Rect2f32& scissor) { m_scissor = scissor; return *this; }
		inline Slice<Vertex const> vertices() const { return m_vertices; }

	private:
		explicit Canvas() {}

		LinearColor m_color = LinearColor::WHITE;
		Rect2f32 m_scissor = Rect2f32::from_center(0, { core::types::F32_MIN, core::types::F32_MAX });

		Array<Vertex> m_vertices;
	};
}