#pragma once

#include "vec2.hpp"

namespace core::math {
	template <typename T>
	class AABB2 {
	public:
		inline constexpr
		AABB2() : m_position(0), m_half_size(0) { }

		inline constexpr static AABB2<T> from_center(Point2<T> location, Vec2<T> half_size) {
			return AABB2<T> { location, half_size };
		}

		inline constexpr static AABB2<T> from_min_max(Point2<T> min, Point2<T> max) {
			const auto half_size = (max - min) / 2;
			const auto position = min + half_size;
			return AABB2<T> { position, half_size };
		}

		NO_DISCARD inline Point2<T> bottom_left() const { return m_position - m_half_size; }
		NO_DISCARD inline Point2<T> top_right() const { return m_position + m_half_size; }

		NO_DISCARD inline Point2<T> bottom_right() const {
			auto result = m_position;
			result.x += m_half_size.x;
			result.y -= m_half_size.y;
			return result;
		}
		NO_DISCARD inline Point2<T> top_left() const {
			auto result = m_position;
			result.x -= m_half_size.x;
			result.y += m_half_size.y;
			return result;
		}

		NO_DISCARD inline Point2<T> position() const { return m_position; }
		NO_DISCARD inline Vec2<T> half_size() const { return m_half_size; }
		NO_DISCARD inline Vec2<T> size() const { return m_half_size * (T)2; }

	private:
		inline constexpr
		AABB2(Point2<T> position, Vec2<T> half_size) : m_position(position), m_half_size(half_size) { }

		Point2<T> m_position;
		Vec2<T> m_half_size;
	};
}

using core::math::AABB2;
using AABB2i32 = AABB2<i32>;
using AABB2u32 = AABB2<u32>;
using AABB2f32 = AABB2<f32>;
using AABB2f64 = AABB2<f64>;

template <typename T>
using Rect2 = AABB2<T>;
using Rect2i32 = Rect2<i32>;
using Rect2u32 = Rect2<u32>;
using Rect2f32 = Rect2<f32>;
using Rect2f64 = Rect2<f64>;