#pragma once

#include "types.hpp"

namespace core::uuid {
	class UUID {
	public:
		inline constexpr UUID(u32 a, u16 b, u16 c, u64 d) : m_a(a), m_b(b), m_c(c), m_d(d) {}

	private:
		u32 m_a;
		u16 m_b;
		u16 m_c;
		u64 m_d;
	};
}
using core::uuid::UUID;