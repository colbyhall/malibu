#pragma once

#include "vec2.hpp"

namespace core::math {
	template <typename T>
	struct Vec3 {
		union {
			struct { T x, y, z; };
			struct { T width, height, depth; };
			struct { T u, v, w; };
            Vec2<T> xy;
			T xyz[3];
		};

		inline constexpr Vec3() : x(0), y(0), z(0) { }
		inline constexpr Vec3(T xyz) : x(xyz), y(xyz), z(xyz) { }
		inline constexpr Vec3(T x, T y, T z) : u(x), v(y), w(z) { }
	};
}

using core::math::Vec3;
using Vec3f32 = Vec3<f32>;
using Vec3f64 = Vec3<f64>;
using Vec3u32 = Vec3<u32>;
using Vec3i32 = Vec3<i32>;

template <typename T>
using Point3 = Vec3<T>;
using Point3f32 = Point3<f32>;
using Point3f64 = Point3<f64>;
using Point3u32 = Point3<u32>;
using Point3i32 = Point3<i32>;