#pragma once

#include "../types.hpp"

namespace core::math {
	struct LinearColor {
		f32 r, g, b, a;

		inline constexpr LinearColor() : r(1), g(1), b(1), a(1) { }
		inline constexpr LinearColor(f32 _r, f32 _g, f32 _b, f32 _a) : r(_r), g(_g), b(_b), a(_a) { }

		static const LinearColor RED;
		static const LinearColor GREEN;
		static const LinearColor BLUE;

		static const LinearColor WHITE;
		static const LinearColor BLACK;
	};

	struct Color {
		union {
			struct { u8 r, g, b, a;  };
			u32 rgba;
		};
	};
}

using core::math::LinearColor;
using core::math::Color;