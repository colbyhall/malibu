#pragma once

#include "../type_traits.hpp"

namespace core { namespace sync {

	template <typename T>
	class Mutex;

	template <typename T>
	class MutexGuard {
	public:
		ALWAYS_INLINE T* operator ->() const { return const_cast<T*>(&m_mutex.m_value); }
		ALWAYS_INLINE T& operator *() const { return const_cast<T&>(m_mutex.m_value); }

	private:
		friend class Mutex<T>;

		ALWAYS_INLINE explicit MutexGuard(const Mutex<T>& mutex) : m_mutex(mutex) {}

		const Mutex<T>& m_mutex;
	};

	template <typename T>
	class Mutex {
	public:
		ALWAYS_INLINE explicit Mutex(T && t) : m_value(core::move(t)) {}

		MutexGuard<T> lock() const {
			return MutexGuard<T>(*this);
		}

	private:
		friend class MutexGuard<T>;

		T m_value;
	};

} }