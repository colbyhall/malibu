#pragma once

#include "../types.hpp"
#include <new>

#define NONE nullptr

namespace core::containers {
	template <typename T>
	class Option {
	public:
		inline constexpr explicit Option() : m_set(false), m_data() {}
		inline constexpr Option(NullPtr) : m_set(false), m_data() {}

		inline Option(T&& t) : m_set(true), m_data() {
			auto* p = m_data;
			new (p) T(core::forward<T>(t));
		}
		inline Option(const T& t) : m_set(true), m_data() {
			auto* p = m_data;
			new (p) T(t);
		}

		NO_DISCARD inline bool is_set() const { return m_set; }
		explicit inline operator bool() const { return is_set(); }

		NO_DISCARD inline T unwrap() {
			VERIFY(is_set());
			m_set = false;

			auto* p = reinterpret_cast<T*>(&m_data[0]);
			return core::move(*p);
		}

		NO_DISCARD inline Option<T&> as_mut() {
			if (is_set()) {
				auto* p = reinterpret_cast<T*>(&m_data[0]);
				return Option<T&>(*p);
			} else {
				return Option<T&>();
			}
		}

		NO_DISCARD inline Option<T const&> as_ref() const {
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
		inline constexpr explicit Option() : m_ptr(nullptr) {}
		inline constexpr Option(T& t) : m_ptr(&t) { }

		NO_DISCARD inline bool is_set() const { return m_ptr != nullptr; }
		explicit inline operator bool() const { return is_set(); }

		NO_DISCARD inline T& unwrap() {
			VERIFY(is_set());
			return *m_ptr;
		}
		
		// TODO: Destructor

	private:
		T* m_ptr;
	};
}
using core::containers::Option;