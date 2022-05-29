#pragma once

#include "../primitives.hpp"
#include "../type_traits.hpp"
#include <new>

namespace core { namespace containers {
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

		NO_DISCARD ALWAYS_INLINE
		Option<T&> as_ref() {
			if (is_set()) {
				auto* p = reinterpret_cast<T*>(&m_data[0]);
				return Option<T&>(*p);
			} else {
				return Option<T&>();
			}
		}

		NO_DISCARD ALWAYS_INLINE
		Option<T const&> as_ref() const {
			if (is_set()) {
				auto* p = reinterpret_cast<T const*>(&m_data[0]);
				return Option<T const&>(*p);
			} else {
				return Option<T const&>();
			}
		}
		
		// TODO: Destructor

	private:
		bool m_set;
		alignas(T) u8 m_data[sizeof(T)];
	};

	template <typename T>
	class Option<T&> {
	public:
		ALWAYS_INLINE constexpr explicit
		Option() : m_ptr(nullptr) {}

		ALWAYS_INLINE constexpr
		Option(T& t) : m_ptr(&t) { }

		NO_DISCARD ALWAYS_INLINE
		bool is_set() const { return m_ptr != nullptr; }

		explicit ALWAYS_INLINE
		operator bool() const { return is_set(); }

		NO_DISCARD ALWAYS_INLINE
		T& unwrap() {
			VERIFY(is_set());
			return *m_ptr;
		}
		
		// TODO: Destructor

	private:
		T* m_ptr;
	};
} }

template <typename T>
using Option = core::containers::Option<T>;