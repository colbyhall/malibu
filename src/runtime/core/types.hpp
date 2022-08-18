#pragma once

#include "templates/remove_reference.hpp"

namespace core::types {
    using u8 = unsigned char;
	constexpr u8 U8_MIN = 0;
	constexpr u8 U8_MAX = 0xff;

    using u16 = unsigned short;
	constexpr u16 U16_MIN = 0;
	constexpr u16 U16_MAX = 0xffff;

    using u32 = unsigned int;
	constexpr u32 U32_MIN = 0;
	constexpr u32 U32_MAX = 0xffffffff;

    using u64 = unsigned long long;
	constexpr u64 U64_MIN = 0;
	constexpr u64 U64_MAX = 0xffffffffffffffff;

    using usize = u64;
	constexpr usize USIZE_MIN = U64_MIN;
	constexpr usize USIZE_MAX = U64_MAX;

    static_assert(sizeof(u8) == 1, "u8 should only be 1 byte");
    static_assert(sizeof(u16) == 2, "u16 should only be 2 bytes");
    static_assert(sizeof(u32) == 4, "u32 should only be 4 bytes");
    static_assert(sizeof(u64) == 8, "u64 should only be 8 bytes");
    static_assert(sizeof(usize) == 8, "usize should only be 8 bytes");

    using i8 = signed char;
	constexpr i8 I8_MIN = (-127 - 1);
	constexpr i8 I8_MAX = 127;

    using i16 = short;
	constexpr i16 I16_MIN = (-32767 - 1);
	constexpr i16 I16_MAX = 32767;

    using i32 = int;
	constexpr i32 I32_MIN = (-2147483647 - 1);
	constexpr i32 I32_MAX = 2147483647;

    using i64 = long long;
	constexpr i64 I64_MIN = (-9223372036854775807 - 1);
	constexpr i64 I64_MAX = 9223372036854775807;

    using isize = i64;
	constexpr isize ISIZE_MIN = I64_MIN;
	constexpr isize ISIZE_MAX = I64_MAX;

    static_assert(sizeof(i8) == 1, "i8 should only be 1 byte");
    static_assert(sizeof(i16) == 2, "i16 should only be 2 bytes");
    static_assert(sizeof(i32) == 4, "i32 should only be 4 bytes");
    static_assert(sizeof(i64) == 8, "i64 should only be 8 bytes");
    static_assert(sizeof(isize) == 8, "isize should only be 8 bytes");

    using f32 = float;
	constexpr f32 F32_MIN = 1.17549435e-38f;
	constexpr f32 F32_MAX = 3.40282347e+38f;

    using f64 = double;

    static_assert(sizeof(f32) == 4, "f32 should only be 4 bytes");
    static_assert(sizeof(f64) == 8, "f64 should only be 8 bytes");

    using NullPtr = decltype(nullptr);
}

using core::types::u8;
using core::types::u16;
using core::types::u32;
using core::types::u64;
using core::types::usize;

using core::types::i8;
using core::types::i16;
using core::types::i32;
using core::types::i64;
using core::types::isize;

using core::types::f32;
using core::types::f64;

using core::types::NullPtr;

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

#define NO_DISCARD [[nodiscard]]
#define NO_RETURN [[noreturn]]
#define ALLOW_UNUSED [[maybe_unused]]

#if COMPILER_MSVC
	#if _MSC_VER < 1300 
		#define DEBUG_TRAP __asm int 3
	#else
		#define DEBUG_TRAP __debugbreak()
	#endif
#else
	#define DEBUG_TRAP __builtin_trap()
#endif

#ifndef BUILD_DEBUG
	#define BUILD_DEBUG 1
#endif

#ifndef BUILD_RELEASE
	#define BUILD_RELEASE 0
#endif

namespace core {
	inline constexpr bool assert(bool val) {
		if (!val) DEBUG_TRAP;
		return true;
	}

	template <typename T>
	constexpr T&& forward(typename RemoveReference<T>::Type& t) {
		return static_cast<T&&>(t);
	}

	template <typename T>
	constexpr T&& forward(typename RemoveReference<T>::Type&& t) {
		return static_cast<T&&>(t);
	}

	template <typename T>
	constexpr typename RemoveReference<T>::Type&& move(T&& t) {
		return static_cast<typename RemoveReference<T>::Type&&>(t);
	}
}

#define NO_COPY(type) type(const type& t) = delete; type& operator=(const type& c) = delete
#define NO_MOVE(type) type(type&& t) noexcept = delete; type& operator=(type&& c) noexcept = delete

#define VERIFY(n) core::assert(n)
#define PANIC(msg) core::assert(false)
#define TODO(msg) core::assert(false)
#define INVALID_CODE_PATH DEBUG_TRAP

#define COUNT_OF(arr) (sizeof(arr[0]) / sizeof(arr))