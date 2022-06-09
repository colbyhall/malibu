#pragma once

#include "math_utility.hpp"

namespace core::math {
	template <typename T>
	struct Quat {
		T x, y, z, w;
	};
} }