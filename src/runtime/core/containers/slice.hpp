#pragma once

#include "non_null.hpp"
#include <initializer_list>

namespace core { namespace containers {
	template <typename T>
	class Slice {
	public:
		ALWAYS_INLINE constexpr
		Slice() : m_ptr(nullptr), m_len(0) {}

		ALWAYS_INLINE constexpr
		Slice(T* data, usize len) : m_ptr(data), m_len(len) {
			// Ensure that if we get no data that we have no len
			VERIFY(data != nullptr|| (data == nullptr && len == 0));
		}

		NO_DISCARD ALWAYS_INLINE
		T* ptr() const { return m_ptr; }

		NO_DISCARD ALWAYS_INLINE
		usize len() const { return m_len; }

		NO_DISCARD ALWAYS_INLINE
		bool is_empty() const { return len() == 0; }

		NO_DISCARD ALWAYS_INLINE
		bool is_valid_index(usize index) const { return index < len(); }

		explicit operator bool() const { return !is_empty(); }

		NO_DISCARD ALWAYS_INLINE
		operator Slice<const T>() const { return Slice<const T>(m_ptr, m_len); }

		NO_DISCARD ALWAYS_INLINE
		T& operator[](usize index) {
			VERIFY(index < len());
			return m_ptr[index];
		}

		NO_DISCARD ALWAYS_INLINE
		const T& operator[](usize index) const {
			VERIFY(index < len());
			return m_ptr[index];
		}

	private:
		T* m_ptr;
		usize m_len;
	};

	template <typename T>
	class Slice<T const> {
	public:
		ALWAYS_INLINE constexpr
		Slice() : m_ptr(nullptr), m_len(0) {}

		ALWAYS_INLINE constexpr
		Slice(T const* data, usize len) : m_ptr(data), m_len(len) {
			// Ensure that if we get no data that we have no len
			VERIFY(data != nullptr|| (data == nullptr && len == 0));
		}

		ALWAYS_INLINE
		Slice(const std::initializer_list<T>& list) 
			: m_ptr(list.begin()), m_len(list.size()) {}

		NO_DISCARD ALWAYS_INLINE
		T const* ptr() const { return m_ptr; }

		NO_DISCARD ALWAYS_INLINE
		usize len() const { return m_len; }

		NO_DISCARD ALWAYS_INLINE
		bool is_empty() const { return len() == 0; }

		NO_DISCARD ALWAYS_INLINE
		bool is_valid_index(usize index) const { return index < len(); }

		explicit operator bool() const { return !is_empty(); }

		ALWAYS_INLINE
		const T& operator[](usize index) const {
			VERIFY(is_valid_index(index));
			return m_ptr[index];
		}

	private:
		T const* m_ptr;
		usize m_len;
	};
} }
template <typename T>
using Slice = core::containers::Slice<T>;