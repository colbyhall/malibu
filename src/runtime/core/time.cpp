#include "time.hpp"

namespace core::time {

	f32 Duration::as_secs_f32() const {
		return (f32)m_secs + ((f32)m_nanos / (f32)NANOS_PER_SEC);
	}

	f64 Duration::as_secs_f64() const {
		return (f64)m_secs + ((f64)m_nanos / (f64)NANOS_PER_SEC);
	}

}