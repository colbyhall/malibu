#include "overlap.hpp"

namespace gui {
	void Overlap::insert_slot(usize index, SharedRef<Element>&& child) {
		Slot slot = core::forward<SharedRef<Element>>(child);
		m_slots.insert(index, core::move(slot));
	}

	void Overlap::insert_slot(
		usize index, 
		SharedRef<Element>&& child, 
		FunctionRef<void(Slot&)>&& add
	) {
		Slot slot = core::forward<SharedRef<Element>>(child);
		add(slot);
		m_slots.insert(index, core::move(slot));
	}

	void Overlap::add_slot(SharedRef<Element>&& child) {
		Slot slot = core::forward<SharedRef<Element>>(child);
		m_slots.push(core::move(slot));
	}

	void Overlap::add_slot(SharedRef<Element>&& child, FunctionRef<void(Slot&)>&& add) {
		Slot slot = core::forward<SharedRef<Element>>(child);
		add(slot);
		m_slots.push(core::move(slot));
	}

	void Overlap::on_layout(const Layout& layout) {
		Element::on_layout(layout);

		for (Slot& slot : m_slots) slot.element_mut()->on_layout(layout);
	}

	void Overlap::on_paint(draw::Canvas& canvas) const {
		for (usize index = 0; index < m_slots.len(); ++index) {
			m_slots[index].element_ref()->on_paint(canvas);
		}
	}

	Option<SlotBase&> Overlap::slot_mut_at(usize index) {
		if (index >= slot_count()) return NONE;
		return m_slots[index];
	}

	usize Overlap::slot_count() const {
		return m_slots.len();
	}
}