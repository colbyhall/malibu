#pragma once

#include "array.hpp"

namespace core { namespace containers {
	class StringView {
	public:
		ALWAYS_INLINE constexpr
		StringView() : m_bytes() {}

		ALWAYS_INLINE constexpr
		StringView(Slice<char const> bytes) : m_bytes(bytes) {}

		// FIXME: Don't use builtin here
		ALWAYS_INLINE constexpr
		StringView(const char* ptr) : m_bytes(Slice<char const>(ptr, __builtin_strlen(ptr))) {}

		NO_DISCARD ALWAYS_INLINE
		Slice<char const> slice() const { return m_bytes; }

	private:
		Slice<char const> m_bytes;
	};

	class String {
	public:
		ALWAYS_INLINE constexpr
		String() : m_bytes() {}

		explicit ALWAYS_INLINE
		String(StringView view) : m_bytes(Array<char>(view.slice())) {}

		NO_DISCARD ALWAYS_INLINE
		Slice<char const> slice() const { return m_bytes.slice(); }

		ALWAYS_INLINE
		operator Slice<char const>() const { return m_bytes.slice(); }
		
	private:
		Array<char> m_bytes;
	};
} }

using StringView = core::containers::StringView;
using String = core::containers::String;