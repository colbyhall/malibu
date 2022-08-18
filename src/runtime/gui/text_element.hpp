#pragma once

#include "element.hpp"
#include "containers/string.hpp"
#include "math/color.hpp"

namespace gui {
	class TextElement : public Element {
		DECLARE_ELEMENT(TextElement, Element);

		void set_size(f32 size);

	protected:
		// Element Interface
		void on_paint(draw::Canvas& canvas) const override;
		// ~Element Interface
	private:
		TextElement(String&& text);

		String m_text;

		const draw::Font& m_font;
		f32 m_size;
		LinearColor m_color;
	};
}