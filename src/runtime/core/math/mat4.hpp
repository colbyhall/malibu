#pragma once

#include "vec4.hpp"

namespace core { namespace math {
	template <typename T>
	struct Mat4 {
		union {
			struct { Vec4<T> x, y, z, w; };
			Vec4<T> columns[4];
			T elements[4 * 4];
		};

		ALWAYS_INLINE constexpr Mat4() : x(0), y(0), z(0), w(0) { }

		static constexpr Mat4<T> identity() {
			Mat4<T> result = {};
			result.elements[0][0] = 1;
			result.elements[1][1] = 1;
			result.elements[2][2] = 1;
			result.elements[3][3] = 1;
			return result;
		}

		static constexpr Mat4<T> from_columns(Vec4<T> x, Vec4<T> y, Vec4<T> z, Vec4<T> w) {
			return { x, y, z, w };
		}

		static constexpr Mat4<T> from_rows(Vec4<T> x, Vec4<T> y, Vec4<T> z, Vec4<T> w) {
			const Vec4<T> x_col = { x.x, y.x, z.x, w.x };
			const Vec4<T> y_col = { x.y, y.y, z.y, w.y };
			const Vec4<T> z_col = { x.z, y.z, z.z, w.z };
			const Vec4<T> w_col = { x.w, y.w, z.w, w.w };
			return { x_col, y_col, z_col, w_col };
		}

		static constexpr Mat4<T> orthographic(T width, T height, T far, T near) {
			// 0 - 1 z clipping
			auto result = Mat4<T>::identity();
			result.x.x = (T)2 / width;
			result.y.y = (T)2 / height;
			result.z.z = (T)1 / (far - near);

			result.w.z = near / (far - near);

			return result;
		}

		static Mat4<T> perspective(T fov, T aspect_ratio, T far, T near) {
			const auto cotan = (T)1 / tan((fov * DEG_TO_RAD) / (T)2);

			auto result = Mat4<T>::identity();
			result.x.x = cotan / aspect_ratio;
			result.y.y = cotan;
			result.z.w = -1;

			result.z.z = far / (near - far);
			result.w.z = -(far * near) / (far - near);

			result.w.w = 0;

			return result;
		}

		NO_DISCARD Vec4<T> row(usize index) const {
			VERIFY(index < 4);
			switch (index) {
				case 0:
					return { x.x, y.x, z.x, w.x };
				case 1:
					return { x.y, y.y, z.y, w.y };
				case 2:
					return { x.z, y.z, z.z, w.z };
				case 3:
					return { x.w, y.w, z.w, w.w };
			}
			return {};
		}

	private:
		ALWAYS_INLINE constexpr Mat4(Vec4<T> _x, Vec4<T> _y, Vec4<T> _z, Vec4<T> _w) : x(_x), y(_y), z(_z), w(_w) { }
	};
} }

template <typename T>
using Mat4 = core::math::Mat4<T>;
using Mat4f32 = Mat4<f32>;
using Mat4f64 = Mat4<f64>;