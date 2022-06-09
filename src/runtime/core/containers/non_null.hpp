#pragma once

#include "../primitives.hpp"

namespace core::containers {
	template <typename T>
	class NonNull {
	public:
		NonNull() = delete;
		NonNull(NullPtr) = delete;
		ALWAYS_INLINE constexpr NonNull(T* ptr) : m_ptr(ptr) {
			VERIFY(m_ptr != nullptr); // Runtime nullptr check
		}

		ALWAYS_INLINE operator T*() const { return m_ptr; }
		ALWAYS_INLINE operator void*() const { return m_ptr; }
		ALWAYS_INLINE T* operator ->() const { return m_ptr; }
		ALWAYS_INLINE T& operator *() const { return *m_ptr; }
		
	private:
		T* m_ptr;
	};

	template <>
	class NonNull<void> {
	public:
		NonNull() = delete;
		NonNull(NullPtr) = delete;

		ALWAYS_INLINE constexpr NonNull(void* ptr) : m_ptr(ptr) {
			VERIFY(m_ptr != nullptr); // Runtime nullptr check
		}

		ALWAYS_INLINE operator void*() const { return m_ptr; }

	private:
		void* m_ptr;
	};

	template <>
	class NonNull<void const> {
	public:
		NonNull() = delete;
		NonNull(NullPtr) = delete;

		ALWAYS_INLINE constexpr NonNull(void const* ptr) : m_ptr(ptr) {
			VERIFY(m_ptr != nullptr); // Runtime nullptr check
		}

		ALWAYS_INLINE operator void const*() const { return m_ptr; }

	private:
		void const* m_ptr;
	};
}

template <typename T>
using NonNull = core::containers::NonNull<T>;
