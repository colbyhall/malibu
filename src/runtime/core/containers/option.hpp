#pragma once

#include "../primitives.hpp"
#include <new>

template <typename T>
class Option {
public:
	ALWAYS_INLINE constexpr explicit
	Option() : m_set(false) {}

	ALWAYS_INLINE
	Option(T&& t) : m_set(true) {
		auto* p = &m_data[0];
		new (p) T(core::move(t));
	}

	ALWAYS_INLINE
	Option(const T& t) : m_set(true) {
		T copy = t;
		auto* p = &m_data[0];
		new (p) T(core::move(copy));
	}

	NO_DISCARD ALWAYS_INLINE
	bool is_set() const { return m_set; }

	explicit ALWAYS_INLINE
	operator bool() const { return is_set(); }

	NO_DISCARD ALWAYS_INLINE
	T unwrap() {
		VERIFY(is_set());
		m_set = false;

		auto* p = reinterpret_cast<T*>(&m_data[0]);
		return core::move(*p);
	}
	
	// TODO: Destructor

private:
	bool m_set;
	alignas(T) u8 m_data[sizeof(T)];
};