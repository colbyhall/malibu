#pragma once

#include "vec2.hpp"

namespace core { namespace math {
	template <typename T>
	class AABB2 {
	public:
		ALWAYS_INLINE constexpr
		AABB2() : m_location(0), m_half_size(0) { }

		ALWAYS_INLINE constexpr
		static AABB2<T> from_center(Point2<T> location, Vec2<T> half_size) {
			return AABB2<T> { location, half_size };
		}

		ALWAYS_INLINE constexpr
		static AABB2<T> from_min_max(Point2<T> min, Point2<T> max) {
			const auto half_size = (max - min) / 2;
			const auto location = min + half_size;
			return AABB2<T> { location, half_size };
		}

	private:
		ALWAYS_INLINE constexpr
		AABB2(Point2<T> location, Vec2<T> half_size) : m_location(location), m_half_size(half_size) { }

		Point2<T> m_location;
		Vec2<T> m_half_size;
	};
} }

template <typename T>
using AABB2 = core::math::AABB2<T>;
using AABB2f32 = AABB2<f32>;

template <typename T>
using Rect2 = AABB2<T>;
using Rect2f32 = Rect2<f32>;