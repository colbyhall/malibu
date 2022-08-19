#pragma once

#include "shape.hpp"
#include "math/aabb2.hpp"
#include "containers/string.hpp"

namespace draw {
	class Font;

	class Text : public Shape {
	public:
		inline Text(StringView text, const AABB2f32& bounds, const Font& font, f32 size)
			: m_text(text), m_bounds(bounds), m_font(font), m_size(size) {}

		inline Text& monospace() { m_monospace = true; }

		// Shape Interface
		void triangulate(Canvas& canvas) const override;
		// ~Shape Interface

	private:
		AABB2f32 m_bounds;
		StringView m_text;
		const Font& m_font;
		f32 m_size;

		bool m_monospace = false;
	};
}