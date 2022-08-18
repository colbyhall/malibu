#pragma once

#include "shape.hpp"
#include "math/aabb2.hpp"

namespace gpu {
	class Texture;
}

namespace draw {
	class Rect : public Shape {
	public:
		inline Rect(const AABB2f32& bounds) : m_bounds(bounds), m_uv0(0), m_uv1(0), m_texture(0) {}

		void set_texture(const gpu::Texture& texture, Vec2f32 uv0 = 0.f, Vec2f32 uv1 = 1.f);

		// Shape Interface
		void triangulate(Canvas& canvas) const override;
		// ~Shape Interface
	private:
		AABB2f32 m_bounds;

		Vec2f32 m_uv0, m_uv1;
		u32 m_texture;
	};
}