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
		inline constexpr Vec4(Vec3<T> xyz, T w) : r(xyz.x), g(xyz.y), b(xyz.z), a(w) { }

		NO_DISCARD inline T dot(const Vec4& rhs) const;

		inline Vec4 operator+(const Vec4& b) const { return { x + b.x, y + b.y, z + b.z, w + b.w }; }
		inline Vec4 operator-(const Vec4& b) const { return { x - b.x, y - b.y, z - b.z, w - b.w }; }
		inline Vec4 operator*(const Vec4& b) const { return { x * b.x, y * b.y, z * b.z, w * b.w }; }
		inline Vec4 operator/(const Vec4& b) const { return { x / b.x, y / b.y, z / b.z, w / b.w }; }
		inline void operator+=(const Vec4& b) { *this = *this + b; }
		inline void operator-=(const Vec4& b) { *this = *this - b; }
		inline void operator*=(const Vec4& b) { *this = *this * b; }
		inline void operator/=(const Vec4& b) { *this = *this / b; }
	};

	template <typename T>
	inline T Vec4<T>::dot(const Vec4<T>& rhs) const {
		return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
	}
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