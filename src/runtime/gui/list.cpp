#include "list.hpp"

namespace gui {
	void List::insert_slot(usize index, SharedRef<Element>&& child) {
		Slot slot = core::forward<SharedRef<Element>>(child);
		m_slots.insert(index, core::move(slot));
	}

	void List::insert_slot(usize index, SharedRef<Element>&& child, FunctionRef<void(Slot&)>&& add) {
		Slot slot = core::forward<SharedRef<Element>>(child);
		add(slot);
		m_slots.insert(index, core::move(slot));
	}

	void List::add_slot(SharedRef<Element>&& child) {
		Slot slot = core::forward<SharedRef<Element>>(child);
		m_slots.push(core::move(slot));
	}

	void List::add_slot(SharedRef<Element>&& child, FunctionRef<void(Slot&)>&& add) {
		Slot slot = core::forward<SharedRef<Element>>(child);
		add(slot);
		m_slots.push(core::move(slot));
	}

	void List::set_direction(Direction direction) {
		if (m_direction != direction) {
			m_direction = direction;

			if (m_layout.is_set()) {
				on_layout(m_layout.as_ref().unwrap());
			}
		}
	}

	void List::on_layout(const Layout& layout) {
		Element::on_layout(layout);

		Vec2f32 size;
		if (m_direction == Direction::Horizontal) 
			size = { layout.local_size.width / (f32)m_slots.len(), layout.local_size.height};
		else 
			size = { layout.local_size.width, layout.local_size.height / (f32)m_slots.len() };


		Point2f32 local_position = layout.local_position;
		for (usize index = 0; index < m_slots.len(); ++index) {
			Layout child = layout;
			child.local_position = local_position;
			child.local_size = size;

			m_slots[index].element_mut()->on_layout(child);

			if (m_direction == Direction::Horizontal) local_position.x += size.width;
			else local_position.y += size.height;
		}
	}

	void List::on_paint(draw::Canvas& canvas) const {
		for (usize index = 0; index < m_slots.len(); ++index) {
			m_slots[index].element_ref()->on_paint(canvas);
		}
	}

	Option<SlotBase&> List::slot_mut_at(usize index) {
		if (index >= slot_count()) return NONE;
		return m_slots[index];
	}

	usize List::slot_count() const {
		return m_slots.len();
	}
}