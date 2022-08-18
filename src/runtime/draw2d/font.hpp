#pragma once

#include "math/vec2.hpp"
#include "fs.hpp"

#include "resources.hpp"

namespace draw2d {
	using namespace core;

	struct Glyph {
		Vec2f32 size;
		Vec2f32 bearing;
		Vec2f32 uv0, uv1;
		f32 advance;
	};

	class Font {
	public:
		static Option<Font> import(fs::PathView path);

		inline Glyph const& glyph(Codepoint c) const {
			const auto index = m_codepoints_to_glyphs[(usize)c];
			if (!m_glphys.is_valid_index(index)) return m_glphys[0];
			return m_glphys[(usize)index];
		}
		inline gpu::Texture const& atlas() const { return m_atlas; }

		inline f32 ascent() const { return m_ascent; }
		inline f32 descent() const { return m_descent; }
		inline f32 line_gap() const { return m_line_gap; }

	private:
		inline Font(
			Array<Glyph>&& glyphs, 
			Array<u32>&& codepoints_to_glyphs, 
			gpu::Texture&& atlas, 
			f32 ascent, 
			f32 descent, 
			f32 line_gap
		) :
			m_glphys(core::move(glyphs)), 
			m_codepoints_to_glyphs(core::move(codepoints_to_glyphs)),
			m_atlas(core::move(atlas)),
			m_ascent(ascent),
			m_descent(descent),
			m_line_gap(line_gap) {}

		Array<Glyph> m_glphys;
		Array<u32> m_codepoints_to_glyphs; // Codepoint to glyph map
		gpu::Texture m_atlas;

		f32 m_ascent;
		f32 m_descent;
		f32 m_line_gap;
	};
}