#include "basic_shapes.hpp"
#include "canvas.hpp"

namespace draw2d {
	void Rect::triangulate(Canvas& canvas) const {
		canvas.add_vertex(m_bounds.bottom_left(), 0.f);
		canvas.add_vertex(m_bounds.top_left(), 0.f);
		canvas.add_vertex(m_bounds.top_right(), 0.f);

		canvas.add_vertex(m_bounds.bottom_left(), 0.f);
		canvas.add_vertex(m_bounds.top_right(), 0.f);
		canvas.add_vertex(m_bounds.bottom_right(), 0.f);
	}
}