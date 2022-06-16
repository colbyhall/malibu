#pragma once

#include "vec3.hpp"

namespace core::math {
	template <typename T>
	struct Vec4 {
		union {
			struct { T x, y, z, w; };
			struct { T r, g, b, a; };
			Vec3<T> xyz;
			T xyzw[4];
		};

		inline constexpr Vec4() : x(0), y(0), z(0), w(0) { }

		inline constexpr Vec4(T xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) { }

		inline constexpr Vec4(T x, T y, T z, T w) : r(x), g(y), b(z), a(w) { }
	};
}

using core::math::Vec4;
using Vec4f32 = Vec4<f32>;
using Vec4f64 = Vec4<f64>;
using Vec4u32 = Vec4<u32>;
using Vec4i32 = Vec4<i32>;

template <typename T>
using Point4 = Vec4<T>;
using Point4f32 = Point4<f32>;
using Point4f64 = Point4<f64>;
using Point4u32 = Point4<u32>;
using Point4i32 = Point4<i32>;