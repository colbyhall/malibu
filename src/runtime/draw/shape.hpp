#pragma once

namespace draw {
	class Canvas;

	class Shape {
	public:
		virtual void triangulate(Canvas& canvas) const = 0;
	};
}