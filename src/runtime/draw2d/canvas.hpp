#pragma once

#include "core.hpp"

#include "math/vec3.hpp"
#include "math/vec2.hpp"
#include "math/color.hpp"

#include "shape.hpp"

namespace draw2d {
	class Canvas {
	public:
		struct Vertex {
			Vec3f32 position;
			LinearColor color;
			Vec2f32 uv;
		};

		static Canvas make() { return Canvas{}; }

		inline Canvas& add_vertex(const Vec3f32& position, const Vec2f32& uv) {
			m_vertices.push({
				position,
				m_color,
				uv
			});
		}

		inline Canvas& paint(const Shape& t) {
			// static_assert(core::is_base_of<Shape, T>, "T must be child of Shape");
			t.triangulate(*this);
			return *this;
		}

		inline Canvas& set_color(const LinearColor& color) { m_color = color; return *this; }

	private:
		explicit Canvas() {}

		LinearColor m_color = LinearColor::WHITE;

		Array<Vertex> m_vertices;
	};
}