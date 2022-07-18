#pragma once

#include "vec3.hpp"
#include "../containers/option.hpp"

namespace core::math {
	template <typename T>
	struct Quat {
		T i, j, k, w;

		inline constexpr Quat() : i(0), j(0), k(0), w(1) {}
		inline explicit Quat(T _i, T _j, T _k, T _w) : i(_i), j(_j), k(_k), w(_w) {}
		static constexpr Quat identity() { return {}; }
		NO_DISCARD static Quat from_axis_angle(Vec3<T> axis, T theta);
		NO_DISCARD static Quat from_euler(T pitch, T yaw, T roll);

		NO_DISCARD inline T len_sq() const { return i * i + j * j + k * k + w * w; }
		NO_DISCARD inline T len() const { return math::sqrt(len_sq()); }
		NO_DISCARD inline bool is_near_zero() const { return len_sq < math::NEAR_ZERO; }
		NO_DISCARD Option<Quat> normalize() const;
		NO_DISCARD inline Quat inverse() const { return Quat{ -i, -j, -k, w }; }
		NO_DISCARD Vec3<T> rotate_vector(const Vec3<T>& xyz) const;

		Quat operator*(const Quat& rhs) const;

	private:
	};

	template <typename T>
	Quat<T> Quat<T>::from_axis_angle(Vec3<T> axis, T theta) {
		const auto half_theta = theta / (T)2;
		const auto s = math::sin(half_theta);
		const auto c = math::cos(half_theta);
		return Quat{ s * axis.x, s * axis.y, s * axis.z, c };
	}

	template <typename T>
	Quat<T> Quat<T>::from_euler(T pitch, T yaw, T roll) {
		const auto rads_div_by_2 = (T)math::DEG_TO_RAD / (T)2;

		const auto pitch1 = math::fmod(pitch, (T)360);
		const auto yaw1 = math::fmod(yaw, (T)360);
		const auto roll1 = math::fmod(roll, (T)360);

		const auto sp = math::sin(pitch1 * rads_div_by_2);
		const auto cp = math::cos(pitch1 * rads_div_by_2);

		const auto sy = math::sin(yaw1 * rads_div_by_2);
		const auto cy = math::cos(yaw1 * rads_div_by_2);

		const auto sr = math::sin(roll1 * rads_div_by_2);
		const auto cr = math::cos(roll1 * rads_div_by_2);

		return Quat{
			cr * sp * sy - sr * cp * cy,
			-cr * sp * cy - sr * cp * sy,
			cr * cp * sy - sr * sp * cy,
			cr * cp * cy + sr * sp * sy
		};
	}

	template <typename T>
	Option<Quat<T>> Quat<T>::normalize() const {
		if (is_near_zero()) return Option<Quat>{};
		const auto inverse = (T)1 / len();
		return Quat{
			i * inverse,
			j * inverse,
			k * inverse,
			w * inverse
		};
	}

	template <typename T>
	Vec3<T> Quat<T>::rotate_vector(const Vec3<T>& xyz) const {
		const Vec3<T> this_xyz = { i, j, k };
		const auto t = this_xyz.cross(xyz) * (T)2;
		return xyz + (t * w) + this_xyz.cross(t);
	}

	template <typename T>
	Quat<T> Quat<T>::operator*(const Quat<T>& rhs) const {
		const auto result = Quat<T> {
			(((w * rhs.i) + (i * rhs.w)) + (j * rhs.k)) - (k * rhs.j),
			(((w * rhs.j) + (j * rhs.w)) + (k * rhs.i)) - (i * rhs.k),
			(((w * rhs.k) + (k * rhs.w)) + (i * rhs.j)) - (j * rhs.i),
			(((w * rhs.w) - (i * rhs.i)) - (j * rhs.j)) - (k * rhs.k)
		};
		return result.normalize().unwrap();
	}
}

using core::math::Quat;
using Quatf32 = Quat<f32>;
using Quatf64 = Quat<f64>;