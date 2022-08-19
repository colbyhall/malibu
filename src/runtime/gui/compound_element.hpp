#pragma once

#include "element.hpp"

namespace gui {
	class CompoundElement : public ContainerElement {
		DECLARE_ABSTRACT_ELEMENT(CompoundElement, ContainerElement);

		class Slot : public SlotBase {
		public:
			inline Slot(SharedRef<Element>&& element)
				: SlotBase(core::forward<SharedRef<Element>>(element)) {}
		private:
		};

		// ContainerElement Interface
		Option<SlotBase&> slot_mut_at(usize index) override {}
		usize slot_count() const override { return 1; }
		// ~ContainerElement Interface

		// Element Interface
		void on_layout(const Layout& layout) override;
		void on_paint(draw::Canvas& canvas) const override;
		// ~Element Interface

	private:
		Option<Slot> m_slot;
	};
}