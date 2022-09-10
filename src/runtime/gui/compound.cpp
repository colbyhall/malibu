#include "compound.hpp"

namespace gui {
	void Compound::on_layout(const Layout& layout) {
		if (m_slot) {
			auto& slot = m_slot.as_mut().unwrap();
			slot.element_mut()->on_layout(layout);
		}
	}

	void Compound::on_paint(draw::Canvas& canvas) const {
		if (m_slot) {
			auto& slot = m_slot.as_ref().unwrap();
			slot.element_ref()->on_paint(canvas);
		}
	}
}