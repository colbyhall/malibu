#include "memory.hpp"

#include <cstdlib>
#include <cstring>

namespace mem {

	NonNull<void> alloc(Layout layout) {
		return std::malloc(layout.size);
	}

	NonNull<void> realloc(NonNull<void> old_ptr, Layout old_layout, Layout new_layout) {
		return std::realloc(old_ptr, new_layout.size);
	}

	void free(NonNull<void> ptr, Layout layout) {
		std::free(ptr);
	}

	NonNull<void> copy(NonNull<void> dest, NonNull<void const> src, usize count) {
		return std::memcpy(dest, src, count);
	}

	NonNull<void> move(NonNull<void> dest, NonNull<void const> src, usize count) {
		return std::memmove(dest, src, count);
	}

	NonNull<void> set(NonNull<void> ptr, u8 value, usize count) {
		return std::memset(ptr, value, count);
	}

}