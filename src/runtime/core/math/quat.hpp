#pragma once

#include "math_utility.hpp"

namespace core { namespace math {
	template <typename T>
	struct Quat {
		T x, y, z, w;
	};
} }