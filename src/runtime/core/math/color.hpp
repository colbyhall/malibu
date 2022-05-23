#pragma once

namespace core { namespace math {
	struct LinearColor {
		f32 r, g, b, a;

		ALWAYS_INLINE constexpr
		LinearColor() : r(1), g(1), b(1), a(1) { }
	};

	struct Color {
		union {
			struct { u8 r, g, b, a;  };
			u32 rgba;
		};
	};
} }

using LinearColor = core::math::LinearColor;
using Color = core::math::Color;