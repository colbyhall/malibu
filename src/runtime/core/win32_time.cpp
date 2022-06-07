#include "time.hpp"
#include "containers/option.hpp"

#define WIN32_LEAN_AND_MEAN
#define WIN32_MEAN_AND_LEAN
#include <windows.h>

namespace core::time {

	// Thread local so we don't have to lock
	static thread_local Option<u64> g_timer_frequency;

	Instant Instant::now() {
		LARGE_INTEGER ticks;
		const auto result = QueryPerformanceCounter(&ticks);
		VERIFY(result);
		return { static_cast<u64>(ticks.QuadPart) };
	}

	Duration Instant::duration_since(Instant earlier) const {
		if (!g_timer_frequency.is_set()) {
			LARGE_INTEGER freq;
			const auto result = QueryPerformanceFrequency(&freq);
			VERIFY(result);
			g_timer_frequency = freq.QuadPart;
		}
		const auto& freq = g_timer_frequency.as_ref().unwrap();

		auto duration =  m_tick - earlier.m_tick;
		const auto secs = duration / freq;
		duration -= secs * freq;
		const auto nanos = (u32)(duration * (NANOS_PER_SEC / freq));

		return Duration(secs, nanos);
	}

	Duration Instant::elapsed() const {
		return Instant::now().duration_since(*this);
	}
}