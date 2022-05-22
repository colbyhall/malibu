#include "memory.hpp"

#include <cstdlib>
#include <cstring>
#include <cstdio>

#define MEMORY_DEBUG 1

namespace core { namespace mem {

	NonNull<void> alloc(Layout layout) {
		void* result = std::malloc(layout.size);
#if MEMORY_DEBUG
		printf("Allocated memory %p\n", result);
#endif
		return result;
	}

	NonNull<void> realloc(NonNull<void> old_ptr, Layout old_layout, Layout new_layout) {
		void* result = std::realloc(old_ptr, new_layout.size);
#if MEMORY_DEBUG
		printf("Reallocated memory old: %p, new: %p\n", old_ptr, result);
#endif
		return result;
	}

	void free(NonNull<void> ptr) {
#if MEMORY_DEBUG
		printf("Freed memory %p\n", ptr);
#endif
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

} }