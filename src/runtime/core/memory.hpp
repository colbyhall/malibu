#pragma once

#include "containers/non_null.hpp"
#include "templates/type_traits.hpp"

namespace core::mem {
	struct Layout {
		usize size;
		usize alignment;
	
		template <typename T>
		static inline constexpr Layout single = { sizeof(T), alignof(T) };
	
		template <typename T>
		static constexpr Layout array(usize len) {
			return Layout{ sizeof(T) * len, alignof(T) };
		}
	};
	
	NonNull<void> alloc(Layout layout);
	template <typename T>
	NonNull<T> alloc(Layout layout = Layout::single<T>) {
		VERIFY(layout.size % sizeof(T) == 0);
		void* ptr = alloc(layout);
		return reinterpret_cast<T*>(ptr);
	}
	NonNull<void> realloc(NonNull<void> old_ptr, Layout old_layout, Layout new_layout);
	void free(NonNull<void> ptr);
	
	NonNull<void> copy(NonNull<void> dst, NonNull<void const> src, usize count);
	NonNull<void> move(NonNull<void> dst, NonNull<void const> src, usize count);
	NonNull<void> set(NonNull<void> ptr, u8 value, usize count);

	u8 count_set_bits_in_byte(u8 b);
	template <typename T>
	inline u32 count_set_bits(T t) {
		const usize size = sizeof(T);
		void* ptr = &t;
		u8 const* u8_casted = (u8 const*)ptr;

		u32 result = 0;
		for (usize i = 0; i < size; ++i) {
			result += count_set_bits_in_byte(u8_casted[i]);
		}

		return result;
	}
}