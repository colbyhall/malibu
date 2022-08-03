#pragma once

#include "vec2.hpp"
#include "../containers/option.hpp"

namespace core::math {
	template <typename T>
	struct alignas(16) Vec3 {
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
		inline constexpr Vec3(Vec2<T> xy, T z = (T)0) : u(xy.x), v(xy.y), w(z) { }

		static constexpr Vec3 forward() { return {1, 0, 0}; }
		static constexpr Vec3 right() { return {0, 1, 0}; }
		static constexpr Vec3 up() { return {0, 0, 1}; }

		static void orthonormal_basis(Vec3<T>& forward, Vec3<T>& right, Vec3<T>& up);

		NO_DISCARD inline T dot(const Vec3& rhs) const;
		NO_DISCARD inline Vec3<T> cross(const Vec3& rhs) const;
		NO_DISCARD inline T len_sq() const { return dot(*this); }
		NO_DISCARD inline T len() const { return math::sqrt(len_sq()); }
		NO_DISCARD inline bool is_near_zero() const { return len_sq() < math::NEAR_ZERO; }
		NO_DISCARD inline Option<Vec3<T>> normalize() const;

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
	void Vec3<T>::orthonormal_basis(Vec3<T>& f, Vec3<T>& r, Vec3<T>& u) {
		// Copied from unreal
		const auto DELTA = (T)0.00001;

		// Project the X and Y axes onto the plane perpendicular to the Z axis.
		f -= u * (f.dot(u) / u.len_sq());
		r -= u * (r.dot(u) / u.len_sq());

		// If the X axis was parallel to the Z axis, choose a vector which is orthogonal to the Y and Z axes.
		if (f.len_sq() < DELTA * DELTA) {
			f = r.cross(u);
		}

		// If the Y axis was parallel to the Z axis, choose a vector which is orthogonal to the X and Z axes.
		if (r.len_sq() < DELTA * DELTA) {
			r = f.cross(r);
		}
	}

	template <typename T> 
	inline T Vec3<T>::dot(const Vec3<T>& rhs) const {
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	template <typename T>
	inline Vec3<T> Vec3<T>::cross(const Vec3<T>& rhs) const {
		return {
			y * rhs.z - rhs.y * z,
			z * rhs.x - rhs.z * x,
			x * rhs.y - rhs.x * y
		};
	}

	template <typename T>
	inline Option<Vec3<T>> Vec3<T>::normalize() const {
		if (is_near_zero()) return Option<Vec3>{};
		const auto inverse = (T)1 / len();
		return Vec3 {
			x * inverse,
			y * inverse,
			z * inverse
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