#pragma once

#include "../primitives.hpp"

template <typename T>
struct Vec2 {
	union {
		struct { T x, y; };
		struct { T width, height; };
		struct { T u, v; };
		T xy[2];
	};

	ALWAYS_INLINE constexpr
	Vec2() : x(0), y(0) { }
	 
	ALWAYS_INLINE constexpr
	Vec2(T xy) : x(xy), y(xy) { }

	ALWAYS_INLINE constexpr
	Vec2(T x, T y) : u(x), v(y) { }
};
using Vec2f32 = Vec2<f32>;
using Vec2f64 = Vec2<f64>;
using Vec2u32 = Vec2<u32>;
using Vec2i32 = Vec2<i32>;

template <typename T>
using Point2 = Vec2<T>;
using Point2f32 = Point2<f32>;
using Point2f64 = Point2<f64>;
using Point2u32 = Point2<u32>;
using Point2i32 = Point2<i32>;