#pragma once

#include "slice.hpp"
#include "option.hpp"
#include "../memory.hpp"
#include "../templates/type_traits.hpp"

#include <new>

namespace core::containers {
	template <typename T>
	class Array {
	public:
		inline constexpr Array() : m_ptr(nullptr), m_len(0), m_cap(0) {}
		inline explicit Array(Slice<const T> slice) : m_ptr(nullptr), m_len(slice.len()), m_cap(0) {
			reserve(slice.len());
			for (int i = 0; i < slice.len(); ++i) {
				T copy = slice[i];
				new (m_ptr + i) T(core::move(copy));
			}
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

		NO_COPY(Array);

		inline Array(Array<T>&& move) noexcept
			: m_ptr(move.m_ptr), m_len(move.m_len), m_cap(move.m_cap) {
			move.m_ptr = nullptr;
			move.m_len = 0;
			move.m_cap = 0;
		}
		inline Array& operator=(Array<T>&& move) noexcept {
			// FIXME: Is this the best way to do this
			Array<T> to_destroy = core::move(*this);
			m_ptr = move.m_ptr;
			m_len = move.m_len;
			m_cap = move.m_cap;
			move.m_ptr = nullptr;
			move.m_len = 0;
			move.m_cap = 0;
			return *this;
		}

		NO_DISCARD inline T* ptr() const { return m_ptr; }
		NO_DISCARD inline usize len() const { return m_len; }
		NO_DISCARD inline usize cap() const { return m_cap; }
		
		inline void set_len(usize len) {
			VERIFY(len <= m_cap);
			m_len = len;
		}

		NO_DISCARD inline bool is_empty() const { return len() == 0; }
		explicit operator bool() const { return !is_empty(); }
		NO_DISCARD inline bool is_valid_index(usize index) const { return index < len(); }
		
		inline operator Slice<T>() { return { m_ptr, m_len }; }
		inline operator Slice<T const>() const { return { m_ptr, m_len }; }

		inline T* begin() { return m_ptr; }
		inline T* end() { return m_ptr + m_len; }
		inline const T* cbegin() const { return m_ptr; }
		inline const T* cend() const { return m_ptr + m_len; }

		NO_DISCARD inline T& operator[](usize index) {
			VERIFY(is_valid_index(index));
			return m_ptr[index];
		}
		NO_DISCARD inline const T& operator[](usize index) const {
			VERIFY(is_valid_index(index));
			return m_ptr[index];
		}

		NO_DISCARD inline Option<T&> last() {
			if (len() > 0) return m_ptr[len() - 1];
			return {};
		}
		NO_DISCARD inline Option<T const&> last() const {
			if (len() > 0) return m_ptr[len() - 1];
			return {};
		}

		void reserve(usize amount) {
			// FIXME: Find proper way of allocating in larger blocks to prevent allocation per push
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

			new (src) T(core::forward<T>(item));
			m_len += 1;
		}

		inline void insert(usize index, const T& item) {
			T copy = item;
			insert(index, core::move(copy));
		}

		inline usize push(T&& item) {
			const auto index = len();
			insert(index, core::move(item));
			return index;
		}
		inline usize push(const T& item) {
			T copy = item;
			return push(core::move(copy));
		}

	private:
		T* m_ptr;
		usize m_len;
		usize m_cap;
	};
}
using core::containers::Array;