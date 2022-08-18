#include "text_element.hpp"
#include "gui.hpp"
#include "canvas.hpp"
#include "text.hpp"

namespace gui {
	TextElement::TextElement(String&& text) :
		m_text(core::move(text)),
		m_font(gui::Manager::the().consola),
		m_size(16.f),
		m_color(LinearColor::WHITE) {}

	void TextElement::set_size(f32 size) {
		m_size = size;
	}

	void TextElement::on_paint(draw::Canvas& canvas) const {
		auto& layout = m_layout.as_ref().unwrap();

		const auto local_position = Vec3f32{ layout.local_position, 1.f };
		const auto local_size = Vec3f32{ layout.local_size, 0.f };

		const auto absolute_position = (layout.local_to_absolute * local_position).xy;
		const auto absolute_size = (layout.local_to_absolute * local_size).xy;

		const auto min = absolute_position;
		const auto max = min + absolute_size;

		canvas.set_color(m_color);

		const auto bounds = Rect2f32::from_min_max(min, max);
		canvas.paint(draw::Text{ m_text, bounds, m_font, m_size });
	}
}