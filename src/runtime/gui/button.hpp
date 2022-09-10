#pragma once

#include "compound.hpp"

namespace gui {
	class Button: public Compound{
		DECLARE_ELEMENT(Button, Compound);

	private:
		Button() = default;
	};
}