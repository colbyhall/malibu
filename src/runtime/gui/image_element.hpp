#pragma once

#include "element.hpp"
#include "resources.hpp"
#include "math/color.hpp"

namespace gui {
	class ImageElement : public Element {
		DECLARE_ELEMENT(ImageElement, Element);

	protected:
		// Element Interface
		void on_paint(draw::Canvas& canvas) const override;
		// ~Element Interface

	private:
		ImageElement(const gpu::Texture& texture, Vec2f32 uv0 = 0.f, Vec2f32 uv1 = 1.f)
			: m_texture(texture.clone()), m_uv0(uv0), m_uv1(uv1), m_color(LinearColor::WHITE) {}

		gpu::Texture m_texture;
		Vec2f32 m_uv0;
		Vec2f32 m_uv1;

		LinearColor m_color;
	};

}