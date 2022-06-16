#pragma once

#include "non_null.hpp"

namespace core::containers {
	template <typename T>
	class Slice {
	public:
		inline constexpr Slice() : m_ptr(nullptr), m_len(0) {}
		inline constexpr Slice(T* data, usize len) : m_ptr(data), m_len(len) {
			// Ensure that if we get no data that we have no len
			VERIFY(data != nullptr|| (data == nullptr && len == 0));
		}

		NO_DISCARD inline T* ptr() const { return m_ptr; }
		NO_DISCARD inline usize len() const { return m_len; }
		NO_DISCARD inline bool is_empty() const { return len() == 0; }
		NO_DISCARD inline bool is_valid_index(usize index) const { return index < len(); }
		explicit operator bool() const { return !is_empty(); }
		NO_DISCARD inline explicit operator Slice<const T>() const { return Slice<const T>(m_ptr, m_len); }

		inline T* begin() { return m_ptr; }
		inline T* end() { return m_ptr + m_len; }
		inline const T* cbegin() const { return m_ptr; }
		inline const T* cend() const { return m_ptr + m_len; }

		NO_DISCARD inline T& operator[](usize index) {
			VERIFY(index < len());
			return m_ptr[index];
		}
		NO_DISCARD inline const T& operator[](usize index) const {
			VERIFY(index < len());
			return m_ptr[index];
		}

		NO_DISCARD Slice<T> shrink(usize amount) const {
			VERIFY(amount <= m_len);
			return { m_ptr, m_len - amount };
		}

	private:
		T* m_ptr;
		usize m_len;
	};

	template <typename T>
	class Slice<T const> {
	public:
		inline constexpr Slice() : m_ptr(nullptr), m_len(0) {}

		inline constexpr Slice(T const* data, usize len) : m_ptr(data), m_len(len) {
			// Ensure that if we get no data that we have no len
			VERIFY(data != nullptr|| (data == nullptr && len == 0));
		}

		NO_DISCARD inline T const* ptr() const { return m_ptr; }
		NO_DISCARD inline usize len() const { return m_len; }
		NO_DISCARD inline bool is_empty() const { return len() == 0; }
		NO_DISCARD inline bool is_valid_index(usize index) const { return index < len(); }
		explicit operator bool() const { return !is_empty(); }

		NO_DISCARD Slice<T const> shrink(usize amount) const {
			VERIFY(amount <= m_len);
			return { m_ptr, m_len - amount };
		}

		inline const T* cbegin() const { return m_ptr; }
		inline const T* cend() const { return m_ptr + m_len; }

		inline const T& operator[](usize index) const {
			VERIFY(is_valid_index(index));
			return m_ptr[index];
		}

	private:
		T const* m_ptr;
		usize m_len;
	};

	template <typename T>
	class Slice<T const&> {
	public:
		inline constexpr Slice() : m_ptr(nullptr), m_len(0) {}

		NO_DISCARD inline T const** ptr() const { return m_ptr; }
		NO_DISCARD inline usize len() const { return m_len; }
		NO_DISCARD inline bool is_empty() const { return len() == 0; }
		NO_DISCARD inline bool is_valid_index(usize index) const { return index < len(); }
		explicit operator bool() const { return !is_empty(); }

		inline const T* cbegin() const { return m_ptr; }
		inline const T* cend() const { return m_ptr + m_len; }

		inline const T& operator[](usize index) const {
			VERIFY(is_valid_index(index));
			return *m_ptr[index];
		}

	private:
		T const** m_ptr;
		usize m_len;
	};
}
using core::containers::Slice;