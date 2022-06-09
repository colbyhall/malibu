#pragma once

#include "array.hpp"

namespace core::containers {
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

		ALWAYS_INLINE explicit operator bool() const { return should_continue(); }
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
		ALWAYS_INLINE const char* operator*() const { return m_bytes.ptr(); }

	private:
		Slice<char const> m_bytes;
	};

	class WStringView {
	public:
		ALWAYS_INLINE constexpr WStringView() : m_chars() {}
		ALWAYS_INLINE constexpr WStringView(Slice<wchar_t const> bytes) : m_chars(bytes) {}
		ALWAYS_INLINE constexpr WStringView(const wchar_t* ptr) : m_chars({ ptr, string_length(ptr) }) {}

		ALWAYS_INLINE operator Slice<wchar_t const>() const { return m_chars; }

		NO_DISCARD ALWAYS_INLINE wchar_t const* ptr() const { return m_chars.ptr(); }
		NO_DISCARD ALWAYS_INLINE usize len() const { return m_chars.len(); }
		ALWAYS_INLINE const wchar_t* operator*() const { return m_chars.ptr(); }

		NO_DISCARD ALWAYS_INLINE const wchar_t* begin() const { return m_chars.cbegin(); }
        NO_DISCARD ALWAYS_INLINE const wchar_t* end() const { return m_chars.cend(); }
        NO_DISCARD ALWAYS_INLINE const wchar_t* cbegin() const { return m_chars.cbegin(); }
		NO_DISCARD ALLOW_UNUSED const wchar_t* cend() const { return m_chars.cend(); }

		NO_DISCARD ALWAYS_INLINE wchar_t operator[](usize index) const { return m_chars[index]; }

	private:
		Slice<wchar_t const> m_chars;
	};

	class String {
	public:
		ALWAYS_INLINE constexpr String() : m_bytes() {}
		explicit ALWAYS_INLINE String(StringView view) : m_bytes(Array<char>(view)) { m_bytes.push(0); }
		explicit ALWAYS_INLINE String(Array<char>&& bytes) : m_bytes(core::forward<Array<char>>(bytes)) {
			if (m_bytes.len() > 0 && m_bytes[m_bytes.len() - 1] != 0) {
				m_bytes.push(0);
			}
		}

		static String from(WStringView string);

		ALWAYS_INLINE operator Slice<char const>() const {
			Slice<char const> result = m_bytes;
			if (m_bytes.len() > 0) {
				result = result.shrink(1);
			}
			return result; 
		}
		ALWAYS_INLINE operator StringView() const { return StringView(m_bytes); }

		NO_DISCARD ALWAYS_INLINE char* ptr() { return m_bytes.ptr(); }
		NO_DISCARD ALWAYS_INLINE char const* ptr() const { return m_bytes.ptr(); }
		ALWAYS_INLINE const char* operator*() const { return m_bytes.ptr(); }

		NO_DISCARD ALWAYS_INLINE usize len() const { return m_bytes.len() > 0 ? m_bytes.len() - 1 : 0; }
		NO_DISCARD ALWAYS_INLINE usize cap() const { return m_bytes.cap(); }
		ALWAYS_INLINE void set_len(usize len) { m_bytes.set_len(len + 1); }

		NO_DISCARD ALWAYS_INLINE Chars chars() const { return Chars(m_bytes); }

		ALWAYS_INLINE void reserve(usize amount) { m_bytes.reserve(amount); }
		String& push(Char c);
		String& push(StringView string);
		
	private:
		Array<char> m_bytes;
	};

	class WString {
	public:
		ALWAYS_INLINE constexpr WString() : m_chars() {}
		ALWAYS_INLINE explicit WString(WStringView view) : m_chars(Array<wchar_t>(view)) {}

		static WString from(StringView string);

		ALWAYS_INLINE operator Slice<wchar_t const>() const { 
			Slice<wchar_t const> result = m_chars;
			if (m_chars.len() > 0) {
				result = result.shrink(1);
			}
			return result; 
		}
		ALWAYS_INLINE operator WStringView() const { return WStringView(m_chars); }

		NO_DISCARD ALWAYS_INLINE wchar_t* ptr() { return m_chars.ptr(); }
		NO_DISCARD ALWAYS_INLINE wchar_t const* ptr() const { return m_chars.ptr(); }
		ALWAYS_INLINE const wchar_t* operator*() const { return m_chars.ptr(); }

		NO_DISCARD ALWAYS_INLINE usize len() const { return m_chars.len() > 0 ? m_chars.len() - 1 : 0; }
		NO_DISCARD ALWAYS_INLINE usize cap() const { return m_chars.cap(); }
		ALWAYS_INLINE void set_len(usize len) { m_chars.set_len(len + 1); }

		ALWAYS_INLINE wchar_t* begin() { return m_chars.begin(); }
		ALWAYS_INLINE wchar_t* end() { return m_chars.end(); }
		ALWAYS_INLINE const wchar_t* cbegin() const { return m_chars.cbegin(); }
		ALWAYS_INLINE const wchar_t* cend() const { return m_chars.cend(); }

		NO_DISCARD ALWAYS_INLINE wchar_t& operator[](usize index) { return m_chars[index]; }
		NO_DISCARD ALWAYS_INLINE wchar_t operator[](usize index) const { return m_chars[index]; }

		ALWAYS_INLINE void reserve(usize amount) { return m_chars.reserve(amount); }
		WString& push(wchar_t w);
		WString& push(WStringView string);
		WString& push(StringView string);

	private:
		Array<wchar_t> m_chars;
	};
}

using Char = core::containers::Char;
using Chars = core::containers::Chars;
using StringView = core::containers::StringView;
using String = core::containers::String;
using WStringView = core::containers::WStringView;
using WString = core::containers::WString;
