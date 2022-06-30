#pragma once

#include "../types.hpp"

namespace core::containers {
	template <typename T>
	class NonNull {
	public:
		NonNull() = delete;
		NonNull(NullPtr) = delete;
		inline constexpr NonNull(T* ptr) : m_ptr(ptr) {
			VERIFY(m_ptr != nullptr); // Runtime nullptr check
		}

		inline operator T*() const { return m_ptr; }
		inline operator void*() const { return m_ptr; }
		inline T* operator ->() const { return m_ptr; }
		inline T& operator *() const { return *m_ptr; }
		
	private:
		T* m_ptr;
	};

	template <>
	class NonNull<void> {
	public:
		NonNull() = delete;
		NonNull(NullPtr) = delete;

		inline constexpr NonNull(void* ptr) : m_ptr(ptr) {
			VERIFY(m_ptr != nullptr); // Runtime nullptr check
		}

		inline operator void*() const { return m_ptr; }

	private:
		void* m_ptr;
	};

	template <>
	class NonNull<void const> {
	public:
		NonNull() = delete;
		NonNull(NullPtr) = delete;

		inline constexpr NonNull(void const* ptr) : m_ptr(ptr) {
			VERIFY(m_ptr != nullptr); // Runtime nullptr check
		}

		inline operator void const*() const { return m_ptr; }

	private:
		void const* m_ptr;
	};
}
using core::containers::NonNull;
