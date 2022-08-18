#pragma once

#include "wstring_view.hpp"
#include "array.hpp"

namespace core::containers {
	class WString {
	public:
		inline constexpr WString() : m_chars() {}

		static inline WString from(WStringView view) {
			WString string;
			string.m_chars = Array<wchar_t>(view);
			return string;
		}
		static WString from(StringView string);

		inline operator Slice<wchar_t const>() const {
			Slice<wchar_t const> result = m_chars;
			if (m_chars.len() > 0) {
				result = result.shrink(1);
			}
			return result; 
		}
		inline operator WStringView() const {
			Slice<wchar_t const> bytes = m_chars;
			bytes.set_len(len());
			return WStringView(bytes);
		}

		NO_DISCARD inline wchar_t* ptr() { return m_chars.ptr(); }
		NO_DISCARD inline wchar_t const* ptr() const { return m_chars.ptr(); }
		inline const wchar_t* operator*() const { return m_chars.ptr(); }

		NO_DISCARD inline usize len() const { return m_chars.len() > 0 ? m_chars.len() - 1 : 0; }
		NO_DISCARD inline usize cap() const { return m_chars.cap(); }
		inline void set_len(usize len) { 
			m_chars[len] = 0;
			m_chars.set_len(len + 1);
		}

		inline wchar_t* begin() { return m_chars.begin(); }
		inline wchar_t* end() { return m_chars.end(); }
		inline const wchar_t* cbegin() const { return m_chars.cbegin(); }
		inline const wchar_t* cend() const { return m_chars.cend(); }

		NO_DISCARD inline wchar_t& operator[](usize index) { return m_chars[index]; }
		NO_DISCARD inline wchar_t operator[](usize index) const { return m_chars[index]; }

		inline void reserve(usize amount) { return m_chars.reserve(amount); }
		WString& push(wchar_t w);
		WString& push(WStringView string);
		WString& push(StringView string);

	private:
		Array<wchar_t> m_chars;
	};
}
using core::containers::WString;
