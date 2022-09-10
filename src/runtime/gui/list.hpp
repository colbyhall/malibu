#pragma once

#include "element.hpp"
#include "containers/array.hpp"

namespace gui {
	enum class Direction {
		Vertical,
		Horizontal
	};

	class List : public Container {
		DECLARE_ELEMENT(List, Container);

		class Slot : public SlotBase {
		public:
			inline Slot(SharedRef<Element>&& element) 
				: SlotBase(core::forward<SharedRef<Element>>(element)) {}
		private:
		};

		void set_direction(Direction direction);

		void insert_slot(usize index, SharedRef<Element>&& child);
		void insert_slot(usize index, SharedRef<Element>&& child, FunctionRef<void(Slot&)>&& add);

		void add_slot(SharedRef<Element>&& child);
		void add_slot(SharedRef<Element>&& child, FunctionRef<void(Slot&)>&& add);

		// Element Interface
		void on_layout(const Layout& layout) override;
		void on_paint(draw::Canvas& canvas) const override;
		// ~Element Interface

		// ContainerElement Interface
		Option<SlotBase&> slot_mut_at(usize index) override;
		usize slot_count() const override;
		// ~ContainerElement Interface

	private:
		Direction m_direction;
		Array<Slot> m_slots;
	};
}