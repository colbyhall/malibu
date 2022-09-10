#pragma once

#include "math/aabb2.hpp"
#include "containers/string.hpp"

namespace gpu {
	class Texture;
}

namespace draw {
	class Canvas;
	class Font;

	class Shape {
	public:
		virtual void triangulate(Canvas& canvas) const = 0;
	};

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