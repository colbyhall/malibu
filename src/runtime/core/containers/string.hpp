#pragma once

#include "array.hpp"

namespace core { namespace containers {
	template<typename T>
	constexpr usize string_length(const T* string) {
		if (*string == 0) return 0;
		string += 1;
		return 1 + string_length(string);
	}

	using Char = u32;
	constexpr Char EOS = 0;
	constexpr Char EOL = '\n';
	constexpr Char UTF8_BOM = 0xfeff;

	class Chars;

	class Chars {
	public:
		ALWAYS_INLINE explicit Chars(Slice<char const> string) : m_string(string), m_index(0), m_decoder_state(0), m_codepoint(0) {}

		ALWAYS_INLINE operator bool() const { return should_continue(); }
		ALWAYS_INLINE Chars& operator++() { next(); return *this; }
		ALWAYS_INLINE Char operator*() const { return get(); }

	private:
		bool should_continue() const;
		void next();
		Char get() const;

		Slice<char const> m_string;
		usize m_index;
		u32 m_decoder_state;
		Char m_codepoint;
	};

	class StringView {
	public:
		ALWAYS_INLINE constexpr StringView() : m_bytes() {}
		ALWAYS_INLINE constexpr StringView(Slice<char const> bytes) : m_bytes(bytes) {}
		ALWAYS_INLINE constexpr StringView(const char* ptr) : m_bytes({ ptr, string_length(ptr) }) {}

		ALWAYS_INLINE operator Slice<char const>() const { return m_bytes; }
		NO_DISCARD ALWAYS_INLINE char const* ptr() const { return m_bytes.ptr(); }
		NO_DISCARD ALWAYS_INLINE usize len() const { return m_bytes.len(); }
		NO_DISCARD ALWAYS_INLINE Chars chars() const { return Chars(m_bytes); }

	private:
		Slice<char const> m_bytes;
	};

	class String {
	public:
		ALWAYS_INLINE constexpr String() : m_bytes() {}
		explicit ALWAYS_INLINE String(StringView view) : m_bytes(Array<char>(view)) {}

		ALWAYS_INLINE operator Slice<char const>() const { return m_bytes; }
		ALWAYS_INLINE operator StringView() const { return StringView(m_bytes); }

		NO_DISCARD ALWAYS_INLINE char const* ptr() const { return m_bytes.ptr(); }
		NO_DISCARD ALWAYS_INLINE usize len() const { return m_bytes.len(); }

		NO_DISCARD ALWAYS_INLINE Chars chars() const { return Chars(m_bytes); }
		
	private:
		Array<char> m_bytes;
	};

	class WideStringView {
	public:
		ALWAYS_INLINE constexpr WideStringView() : m_chars() {}
		ALWAYS_INLINE constexpr WideStringView(Slice<wchar_t const> bytes) : m_chars(bytes) {}
		ALWAYS_INLINE constexpr WideStringView(const wchar_t* ptr) : m_chars({ ptr, string_length(ptr) }) {}

		ALWAYS_INLINE operator Slice<wchar_t const>() const { return m_chars; }

	private:
		Slice<wchar_t const> m_chars;
	};

	class WideString {
	public:
		ALWAYS_INLINE constexpr WideString() : m_chars() {}
		ALWAYS_INLINE explicit WideString(WideStringView view) : m_chars(Array<wchar_t>(view)) {}

		ALWAYS_INLINE operator Slice<wchar_t const>() const { return m_chars; }
		ALWAYS_INLINE operator WideStringView() const { return WideStringView(m_chars); }

	private:
		Array<wchar_t> m_chars;
	};
} }

using Char = core::containers::Char;
using Chars = core::containers::Chars;
using StringView = core::containers::StringView;
using String = core::containers::String;
