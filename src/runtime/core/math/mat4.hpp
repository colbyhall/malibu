#pragma once

#include "vec4.hpp"
#include "quat.hpp"

namespace core::math {
	template <typename T>
	struct Mat4 {
		union {
			struct { Vec4<T> x, y, z, w; };
			Vec4<T> columns[4];
			T elements[4 * 4];
		};

		inline constexpr Mat4() : x(0), y(0), z(0), w(0) {}
		static NO_DISCARD constexpr Mat4 from_columns(Vec4<T> x, Vec4<T> y, Vec4<T> z, Vec4<T> w);			    
		static NO_DISCARD constexpr Mat4 from_rows(Vec4<T> x, Vec4<T> y, Vec4<T> z, Vec4<T> w);
		static NO_DISCARD constexpr Mat4 orthographic(T width, T height, T near, T far);
		static NO_DISCARD Mat4 perspective(T fov, T aspect_ratio, T near, T far);
		static NO_DISCARD Mat4 transform(Vec3<T> position, Quat<T> rotation, Vec3<T> scale);
		static NO_DISCARD constexpr Mat4 identity();

		NO_DISCARD Option<Mat4> inverse() const;

		NO_DISCARD Vec4<T> row(usize index) const;

		Mat4 operator*(const Mat4& rhs) const;
		Vec4<T> operator*(const Vec4<T>& rhs) const;

	private:
		inline constexpr Mat4(Vec4<T> _x, Vec4<T> _y, Vec4<T> _z, Vec4<T> _w) 
			: x(_x), y(_y), z(_z), w(_w) {}
	};

	template <typename T>
	constexpr Mat4<T> Mat4<T>::identity() {
		return Mat4<T>::from_columns(
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 1 }
		);
	}

	template <typename T>
	constexpr Mat4<T> Mat4<T>::from_columns(Vec4<T> x, Vec4<T> y, Vec4<T> z, Vec4<T> w) {
		return { x, y, z, w };
	}

	template <typename T>
	constexpr Mat4<T> Mat4<T>::from_rows(Vec4<T> x, Vec4<T> y, Vec4<T> z, Vec4<T> w) {
		const Vec4<T> x_col = { x.x, y.x, z.x, w.x };
		const Vec4<T> y_col = { x.y, y.y, z.y, w.y };
		const Vec4<T> z_col = { x.z, y.z, z.z, w.z };
		const Vec4<T> w_col = { x.w, y.w, z.w, w.w };
		return { x_col, y_col, z_col, w_col };
	}

	template <typename T>
	constexpr Mat4<T> Mat4<T>::orthographic(T width, T height, T near, T far) {
		// 0 - 1 z clipping
		auto result = Mat4::identity();
		result.x.x = (T)2 / width;
		result.y.y = (T)2 / height;
		result.z.z = (T)1 / (far - near);

		result.w.z = near / (far - near);

		return result;
	}

	template <typename T>
	Mat4<T> Mat4<T>::perspective(T fov, T aspect_ratio, T near, T far) {
		const auto cotan = (T)1 / tan((fov * DEG_TO_RAD) / (T)2);

		auto result = Mat4::identity();
		result.x.x = cotan / aspect_ratio;
		result.y.y = cotan;
		result.z.w = -1;

		result.z.z = far / (near - far);
		result.w.z = -(far * near) / (far - near);

		result.w.w = 0;

		return result;
	}

	template <typename T>
	Mat4<T> Mat4<T>::transform(Vec3<T> position, Quat<T> rotation, Vec3<T> scale) {
		Mat4 result;
		result.x = { rotation.rotate_vector(Vec3f32::forward()) * scale.x, 0 };
		result.y = { rotation.rotate_vector(Vec3f32::right()) * scale.y, 0 };
		result.z = { rotation.rotate_vector(Vec3f32::up()) * scale.z, 0 };
		result.w = { position, 1 };
		return result;
	}

	template <typename T>
	Option<Mat4<T>> Mat4<T>::inverse() const {
		Mat4 result = {};
		result.elements[0] = elements[5] * elements[10] * elements[15]
			- elements[5] * elements[11] * elements[14]
			- elements[9] * elements[6] * elements[15]
			+ elements[9] * elements[7] * elements[14]
			+ elements[13] * elements[6] * elements[11]
			- elements[13] * elements[7] * elements[10];

		result.elements[4] = -elements[4] * elements[10] * elements[15]
			+ elements[4] * elements[11] * elements[14]
			+ elements[8] * elements[6] * elements[15]
			- elements[8] * elements[7] * elements[14]
			- elements[12] * elements[6] * elements[11]
			+ elements[12] * elements[7] * elements[10];

		result.elements[8] = elements[4] * elements[9] * elements[15]
			- elements[4] * elements[11] * elements[13]
			- elements[8] * elements[5] * elements[15]
			+ elements[8] * elements[7] * elements[13]
			+ elements[12] * elements[5] * elements[11]
			- elements[12] * elements[7] * elements[9];

		result.elements[12] = -elements[4] * elements[9] * elements[14]
			+ elements[4] * elements[10] * elements[13]
			+ elements[8] * elements[5] * elements[14]
			- elements[8] * elements[6] * elements[13]
			- elements[12] * elements[5] * elements[10]
			+ elements[12] * elements[6] * elements[9];

		result.elements[1] = -elements[1] * elements[10] * elements[15]
			+ elements[1] * elements[11] * elements[14]
			+ elements[9] * elements[2] * elements[15]
			- elements[9] * elements[3] * elements[14]
			- elements[13] * elements[2] * elements[11]
			+ elements[13] * elements[3] * elements[10];

		result.elements[5] = elements[0] * elements[10] * elements[15]
			- elements[0] * elements[11] * elements[14]
			- elements[8] * elements[2] * elements[15]
			+ elements[8] * elements[3] * elements[14]
			+ elements[12] * elements[2] * elements[11]
			- elements[12] * elements[3] * elements[10];

		result.elements[9] = -elements[0] * elements[9] * elements[15]
			+ elements[0] * elements[11] * elements[13]
			+ elements[8] * elements[1] * elements[15]
			- elements[8] * elements[3] * elements[13]
			- elements[12] * elements[1] * elements[11]
			+ elements[12] * elements[3] * elements[9];

		result.elements[13] = elements[0] * elements[9] * elements[14]
			- elements[0] * elements[10] * elements[13]
			- elements[8] * elements[1] * elements[14]
			+ elements[8] * elements[2] * elements[13]
			+ elements[12] * elements[1] * elements[10]
			- elements[12] * elements[2] * elements[9];

		result.elements[2] = elements[1] * elements[6] * elements[15]
			- elements[1] * elements[7] * elements[14]
			- elements[5] * elements[2] * elements[15]
			+ elements[5] * elements[3] * elements[14]
			+ elements[13] * elements[2] * elements[7]
			- elements[13] * elements[3] * elements[6];

		result.elements[6] = -elements[0] * elements[6] * elements[15]
			+ elements[0] * elements[7] * elements[14]
			+ elements[4] * elements[2] * elements[15]
			- elements[4] * elements[3] * elements[14]
			- elements[12] * elements[2] * elements[7]
			+ elements[12] * elements[3] * elements[6];

		result.elements[10] = elements[0] * elements[5] * elements[15]
			- elements[0] * elements[7] * elements[13]
			- elements[4] * elements[1] * elements[15]
			+ elements[4] * elements[3] * elements[13]
			+ elements[12] * elements[1] * elements[7]
			- elements[12] * elements[3] * elements[5];

		result.elements[14] = -elements[0] * elements[5] * elements[14]
			+ elements[0] * elements[6] * elements[13]
			+ elements[4] * elements[1] * elements[14]
			- elements[4] * elements[2] * elements[13]
			- elements[12] * elements[1] * elements[6]
			+ elements[12] * elements[2] * elements[5];

		result.elements[3] = -elements[1] * elements[6] * elements[11]
			+ elements[1] * elements[7] * elements[10]
			+ elements[5] * elements[2] * elements[11]
			- elements[5] * elements[3] * elements[10]
			- elements[9] * elements[2] * elements[7]
			+ elements[9] * elements[3] * elements[6];

		result.elements[7] = elements[0] * elements[6] * elements[11]
			- elements[0] * elements[7] * elements[10]
			- elements[4] * elements[2] * elements[11]
			+ elements[4] * elements[3] * elements[10]
			+ elements[8] * elements[2] * elements[7]
			- elements[8] * elements[3] * elements[6];

		result.elements[11] = -elements[0] * elements[5] * elements[11]
			+ elements[0] * elements[7] * elements[9]
			+ elements[4] * elements[1] * elements[11]
			- elements[4] * elements[3] * elements[9]
			- elements[8] * elements[1] * elements[7]
			+ elements[8] * elements[3] * elements[5];

		result.elements[15] = elements[0] * elements[5] * elements[10]
			- elements[0] * elements[6] * elements[9]
			- elements[4] * elements[1] * elements[10]
			+ elements[4] * elements[2] * elements[9]
			+ elements[8] * elements[1] * elements[6]
			- elements[8] * elements[2] * elements[5];

		const auto det = elements[0] * result.elements[0] + elements[1] * result.elements[4] + elements[2] * result.elements[8] + elements[3] * result.elements[12];
		if (det == 0.f ) {
			return Option<Mat4>{};
		}
		const auto det_reciprocal = (T)1 / det;

		for (int i = 0; i < 16; ++i) {
			result.elements[i] *= det_reciprocal;
		}

		return result;
	}

	template <typename T>
	Vec4<T> Mat4<T>::row(usize index) const {
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

	template <typename T>
	Mat4<T> Mat4<T>::operator*(const Mat4<T>& rhs) const {
		Vec4<T> row_x = {};
		const auto row0 = row(0);
		row_x.x = row0.dot(rhs.x);
		row_x.y = row0.dot(rhs.y);
		row_x.z = row0.dot(rhs.z);
		row_x.w = row0.dot(rhs.w);

		Vec4<T> row_y = {};
		const auto row1 = row(1);
		row_y.x = row1.dot(rhs.x);
		row_y.y = row1.dot(rhs.y);
		row_y.z = row1.dot(rhs.z);
		row_y.w = row1.dot(rhs.w);

		Vec4<T> row_z = {};
		const auto row2 = row(2);
		row_z.x = row2.dot(rhs.x);
		row_z.y = row2.dot(rhs.y);
		row_z.z = row2.dot(rhs.z);
		row_z.w = row2.dot(rhs.w);

		Vec4<T> row_w = {};
		const auto row3 = row(3);
		row_w.x = row3.dot(rhs.x);
		row_w.y = row3.dot(rhs.y);
		row_w.z = row3.dot(rhs.z);
		row_w.w = row3.dot(rhs.w);

		return Mat4::from_rows(row_x, row_y, row_z, row_w);
	}

	template <typename T>
	Vec4<T> Mat4<T>::operator*(const Vec4<T>& rhs) const {
		const auto x = row(0).dot(rhs);
		const auto y = row(1).dot(rhs);
		const auto z = row(2).dot(rhs);
		const auto w = row(3).dot(rhs);

		return { x, y, z, w };
	}
}

using core::math::Mat4;
using Mat4f32 = Mat4<f32>;
using Mat4f64 = Mat4<f64>;