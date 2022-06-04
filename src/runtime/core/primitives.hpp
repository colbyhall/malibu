#pragma once

#include <cstdint>
#include <cassert> // FIXME: Replace this

namespace core {
    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using usize = u64;

    static_assert(sizeof(u8) == 1, "u8 should only be 1 byte");
    static_assert(sizeof(u16) == 2, "u16 should only be 2 bytes");
    static_assert(sizeof(u32) == 4, "u32 should only be 4 bytes");
    static_assert(sizeof(u64) == 8, "u64 should only be 8 bytes");
    static_assert(sizeof(usize) == 8, "usize should only be 8 bytes");

    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;
    using isize = i64;

    static_assert(sizeof(i8) == 1, "i8 should only be 1 byte");
    static_assert(sizeof(i16) == 2, "i16 should only be 2 bytes");
    static_assert(sizeof(i32) == 4, "i32 should only be 4 bytes");
    static_assert(sizeof(i64) == 8, "i64 should only be 8 bytes");
    static_assert(sizeof(isize) == 8, "isize should only be 8 bytes");

    using f32 = float;
    using f64 = double;

    static_assert(sizeof(f32) == 4, "f32 should only be 4 bytes");
    static_assert(sizeof(f64) == 8, "f64 should only be 8 bytes");

    using NullPtr = decltype(nullptr);
};

#define ALWAYS_INLINE __forceinline
#define NO_DISCARD [[nodiscard]]
#define NO_RETURN [[noreturn]]
#define ALLOW_UNUSED [[maybe_unused]]

#if defined(_WIN32) || defined(_WIN64)
	#define PLATFORM_WIN32 1
#elif defined(__APPLE__) && defined(__MACH__)
	#define PLATFORM_OSX 1
#elif defined(__unix__)
	#define PLATFORM_UNIX 1
	#if defined(__linux__)
		#define PLATFORM_LINUX 1
	#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
		#define PLATFORM_FREEBSD 1
	#else
		#error This UNIX Operating System is not supported
	#endif
#else
	#error Operating System is not supported
#endif

#ifndef PLATFORM_WIN32
	#define PLATFORM_WIN32 0
#endif
#ifndef PLATFORM_OSX
	#define PLATFORM_OSX 0
#endif
#ifndef PLATFORM_UNIX
	#define PLATFORM_UNIX 0
#endif
#ifndef PLATFORM_LINUX
	#define PLATFORM_LINUX 0
#endif
#ifndef PLATFORM_FREEBSD
	#define PLATFORM_FREEBSD 0
#endif

#if defined(_MSC_VER)
	#define COMPILER_MSVC 1
#elif defined(__GNUC__)
	#define COMPILER_GCC 1
#elif defined(__clang__)
	#define COMPILER_CLANG 1
#else
	#error Compiler is not supported
#endif

#ifndef COMPILER_MSVC
	#define COMPILER_MSVC 0
#endif
#ifndef COMPILER_GCC
	#define COMPILER_GCC 0
#endif
#ifndef COMPILER_CLANG
	#define COMPILER_CLANG 0
#endif

#if COMPILER_MSVC
	#if _MSC_VER < 1300 
		#define DEBUG_TRAP __asm int 3
	#else
		#define DEBUG_TRAP __debugbreak()
	#endif
#else
	#define DEBUG_TRAP __builtin_trap()
#endif

#define VERIFY(n) assert(n)
#define PANIC(msg) assert(true)
#define TODO(msg) assert(true)
#define INVALID_CODE_PATH DEBUG_TRAP

using u8 = core::u8;
using u16 = core::u16;
using u32 = core::u32;
using u64 = core::u64;
using usize = core::usize;

using i8 = core::i8;
using i16 = core::i16;
using i32 = core::i32;
using i64 = core::i64;
using isize = core::isize;

using f32 = core::f32;
using f64 = core::f64;