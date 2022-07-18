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

		static constexpr Vec3 forward() { return {1, 0, 0}; }
		static constexpr Vec3 right() { return {0, 1, 0}; }
		static constexpr Vec3 up() { return {0, 0, 1}; }

		NO_DISCARD inline T dot(const Vec3& rhs) const;
		NO_DISCARD Vec3<T> cross(const Vec3& rhs) const;
		NO_DISCARD inline T len_sq() const { return dot(*this); }
		NO_DISCARD inline T len() const { return math::sqrt(len_sq()); }

		template <typename D>
		inline Vec3<D> cast() const { return { (D)x, (D)y, (D)z }; }

		inline Vec3 operator+(const Vec3& b) const { return { x + b.x, y + b.y, z + b.z }; }
		inline Vec3 operator-(const Vec3& b) const { return { x - b.x, y - b.y, z - b.z }; }
		inline Vec3 operator*(const Vec3& b) const { return { x * b.x, y * b.y, z * b.z }; }
		inline Vec3 operator/(const Vec3& b) const { return { x / b.x, y / b.y, z / b.z }; }
		inline void operator+=(const Vec3& b) { *this = *this + b; }
		inline void operator-=(const Vec3& b) { *this = *this - b; }
		inline void operator*=(const Vec3& b) { *this = *this * b; }
		inline void operator/=(const Vec3& b) { *this = *this / b; }

		inline Vec3 operator-() const { return { -x, -y, -z }; }
	};

	template <typename T> 
	inline T Vec3<T>::dot(const Vec3<T>& rhs) const {
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	template <typename T>
	Vec3<T> Vec3<T>::cross(const Vec3<T>& rhs) const {
		return {
			y * rhs.z - rhs.y * z,
			z * rhs.x - rhs.z * x,
			x * rhs.y - rhs.x * y
		};
	}
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