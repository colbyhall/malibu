#pragma once

#include "slice.hpp"
#include "../memory.hpp"
#include "../type_traits.hpp"

#include <initializer_list>
#include <new>

namespace core { namespace containers {
	template <typename T>
	class Array {
	public:
		ALWAYS_INLINE constexpr
		Array() : m_ptr(nullptr), m_len(0), m_cap(0) {}

		ALWAYS_INLINE explicit
		Array(Slice<const T> slice) : m_len(slice.len()) {
			reserve(slice.len());
			for (int i = 0; i < slice.len(); ++i) {
				T copy = slice[i];
				new (m_ptr + i) T(core::move(copy));
			}
		}

		NO_COPY(Array);

		ALWAYS_INLINE
		Array(Array<T>&& move) noexcept : m_ptr(move.m_ptr), m_len(move.m_len), m_cap(move.m_cap) {
			move.m_ptr = nullptr;
			move.m_len = 0;
			move.m_cap = 0;
		}

		ALWAYS_INLINE
		Array& operator=(Array<T>&& m) noexcept {
			// FIXME: Is this the best way to do this
			Array<T> to_destroy = move(*this);
			m_ptr = m.m_ptr;
			m_len = m.m_len;
			m_cap = m.m_cap;
			m.m_data = nullptr;
			m.m_len = 0;
			m.m_cap = 0;
			return *this;
		}

		~Array() {
			if (m_ptr) {
				for (usize i = 0; i < m_len; ++i) {
					T& item = m_ptr[i];
					item.~T();
				}
				mem::free(m_ptr);
				m_ptr = nullptr;
			}
		}

		NO_DISCARD ALWAYS_INLINE
		usize len() const { return m_len; }

		NO_DISCARD ALWAYS_INLINE
		usize cap() const { return m_cap; }

		NO_DISCARD ALWAYS_INLINE
		bool is_empty() const { return len() == 0; }

		explicit operator bool() const { return !is_empty(); }
		
		NO_DISCARD ALWAYS_INLINE
		Slice<T> slice() { return Slice<T>(m_ptr, m_len); }

		NO_DISCARD ALWAYS_INLINE
		Slice<T const> slice() const { return Slice<T>(m_ptr, m_len); }

		NO_DISCARD ALWAYS_INLINE
		bool is_valid_index(usize index) const { return index < len(); }

		NO_DISCARD ALWAYS_INLINE
		T& operator[](usize index) {
			VERIFY(is_valid_index(index));
			return m_ptr[index];
		}

		NO_DISCARD ALWAYS_INLINE
		const T& operator[](usize index) const {
			VERIFY(is_valid_index(index));
			return m_ptr[index];
		}

		void reserve(usize amount) {
			// FIXME: Find proper way of allocating in larger blocks to prevent allocation per push
			auto c = cap();
			const auto old_cap = m_cap;
			auto new_cap = old_cap + amount;
			while (m_cap < new_cap) {
				m_cap += new_cap >> 1;
				m_cap += 1;
			}

			// FIXME: Custom allocator support
			if (m_ptr == nullptr) {
				void* ptr = mem::alloc(mem::Layout::array<T>(m_cap));
				m_ptr = static_cast<T*>(ptr);
			} else {
				void* ptr = mem::realloc(m_ptr, mem::Layout::array<T>(old_cap), mem::Layout::array<T>(m_cap));
				m_ptr = static_cast<T*>(ptr);
			}
		}

		void insert(usize index, T&& item) {
			VERIFY(index <= m_len);
			if (len() == cap()) reserve(1);

			auto* src = m_ptr + index;
			if (index != len()) {
				mem::move(src + 1, src, (len() - index) * sizeof(T));
				mem::set(src, 0, sizeof(T));
			}

			new (src) T(core::move(item));
			m_len += 1;
		}

		ALWAYS_INLINE
		void insert(usize index, const T& item) {
			T copy = item;
			insert(index, core::move(copy));
		}

		ALWAYS_INLINE
		usize push(T&& item) {
			const auto index = len();
			insert(index, core::move(item));
			return index;
		}

	private:
		T* m_ptr;
		usize m_len;
		usize m_cap;
	};
} }

template <typename T>
using Array = core::containers::Array<T>;