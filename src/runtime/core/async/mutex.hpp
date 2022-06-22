#pragma once

#include "../type_traits.hpp"

namespace core::async {

	template <typename T>
	class Mutex;

	template <typename T>
	class MutexGuard {
	public:
		inline T* operator ->() const { return const_cast<T*>(&m_mutex.m_value); }
		inline T& operator *() const { return const_cast<T&>(m_mutex.m_value); }

	private:
		friend class Mutex<T>;

		inline explicit MutexGuard(const Mutex<T>& mutex) : m_mutex(mutex) {}

		const Mutex<T>& m_mutex;
	};

	template <typename T>
	class Mutex {
	public:
		inline explicit Mutex(T && t) : m_value(core::move(t)) {}

		MutexGuard<T> lock() const {
			return MutexGuard<T>(*this);
		}

	private:
		friend class MutexGuard<T>;

		T m_value;
	};

}