#pragma once

#include "compound_element.hpp"

namespace gui {
	class ButtonElement : public CompoundElement {
		DECLARE_ELEMENT(ButtonElement, CompoundElement);

	private:
		ButtonElement() = default;
	};
}