#pragma once

namespace draw2d {
	class Canvas;

	class Shape {
	public:
		virtual void triangulate(Canvas& canvas) const = 0;
	};
}