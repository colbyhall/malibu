#pragma once

#include "shape.hpp"
#include "math/aabb2.hpp"

namespace draw2d {
	class Rect : public Shape {
	public:
		Rect(const AABB2f32& bounds) : m_bounds(bounds) {}

		// Shape Interface
		void triangulate(Canvas& canvas) const override;
	private:
		AABB2f32 m_bounds;
	};
}