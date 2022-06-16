#pragma once

#include "primitives.hpp"

namespace core::time {
	constexpr u64 NANOS_PER_SEC = 1000000000;
	constexpr u64 NANOS_PER_MILLI = 1000000;
	constexpr u64 NANOS_PER_MICRO = 1000;
	constexpr u64 MILLIS_PER_SEC = 1000;
	constexpr u64 MICROS_PER_SEC = 1000000;

	class Duration {
	public:
		inline constexpr explicit Duration(u64 secs, u32 nanos) : m_secs(secs), m_nanos(nanos) { }

		NO_DISCARD f32 as_secs_f32() const;
        NO_DISCARD f64 as_secs_f64() const;

	private:
		u64 m_secs;
		u32 m_nanos;
	};

	class Instant {
	public:
		static Instant now();

		Duration duration_since(Instant earlier) const;
		Duration elapsed() const;

	private:
		inline constexpr Instant(u64 ticks) : m_tick(ticks) { }

		u64 m_tick;
	};
}