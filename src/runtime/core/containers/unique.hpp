#pragma once

#include "type_traits.hpp"
#include "memory.hpp"

#include <new>

template <typename T>
class Unique {
public:
	explicit
	Unique(T&& t) {
		void* ptr = mem::alloc(mem::Layout::single<T>());
		new (ptr) T(core::forward(t)); 
		m_ptr = static_cast<T*>(ptr);
	}

	explicit
	Unique(const T& t) {
		T copy = t;
		void* ptr = mem::malloc(mem::Layout::single<T>());
		new (ptr) T(core::move(copy));
		m_ptr = static_cast<T*>(ptr);
	}

	template<typename D>
	Unique(Unique<D>&& move) noexcept : m_ptr(move.m_ptr) {
		move.m_ptr = nullptr;
	}

	NO_COPY(Unique);

	Unique(Unique<T>&& move) noexcept : m_ptr(move.m_ptr) {
		move.m_ptr = nullptr;
	}
	Unique& operator=(Unique<T>&& m) noexcept {
		Unique<T> to_destroy = core::move(*this);
		m_ptr = m.m_ptr;
		m.m_ptr = nullptr;
		return *this;
	}

	NO_DISCARD ALWAYS_INLINE explicit
	operator NonNull<T>() { return m_ptr; }

	NO_DISCARD ALWAYS_INLINE explicit
	operator NonNull<T const>() const { return m_ptr; }

	NO_DISCARD ALWAYS_INLINE explicit
	operator T*() { return m_ptr; }

	NO_DISCARD ALWAYS_INLINE explicit
	operator T const *() const { return m_ptr; }

	NO_DISCARD ALWAYS_INLINE
	T* operator ->() { return m_ptr; }

	NO_DISCARD ALWAYS_INLINE
	T const* operator ->() const { return m_ptr; }

	NO_DISCARD ALWAYS_INLINE
	T& operator *() { return *m_ptr; }

	NO_DISCARD ALWAYS_INLINE
	T const& operator *() const { return *m_ptr; }

private:
	T* m_ptr;

	Unique() = default;
};