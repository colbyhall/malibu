#pragma once

#include "../primitives.hpp"

namespace core::math {
	struct LinearColor {
		f32 r, g, b, a;

		ALWAYS_INLINE constexpr LinearColor() : r(1), g(1), b(1), a(1) { }
		ALWAYS_INLINE constexpr LinearColor(f32 _r, f32 _g, f32 _b, f32 _a) : r(_r), g(_g), b(_b), a(_a) { }

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

using LinearColor = core::math::LinearColor;
using Color = core::math::Color;