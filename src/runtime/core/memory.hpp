#pragma once

#include "containers/non_null.hpp"

namespace core { namespace mem {
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
	NonNull<void> realloc(NonNull<void> old_ptr, Layout old_layout, Layout new_layout);
	void free(NonNull<void> ptr);
	
	NonNull<void> copy(NonNull<void> dest, NonNull<void const> src, usize count);
	NonNull<void> move(NonNull<void> dest, NonNull<void const> src, usize count);
	NonNull<void> set(NonNull<void> ptr, u8 value, usize count);
} }