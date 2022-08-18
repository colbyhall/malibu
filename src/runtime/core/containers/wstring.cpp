#include "wstring.hpp"
#include "string_view.hpp"

namespace core::containers {
	static wchar_t utf32_to_utf16(Codepoint c) {
		u32 h;
		u32 l;

		if (c < 0x10000) {
			h = 0;
			l = c;
			return c;
		}
		Codepoint t = c - 0x10000;
		h = (((t<<12)>>22) + 0xD800);
		l = (((t<<22)>>22) + 0xDC00);
		Codepoint ret = ((h<<16) | ( l & 0x0000FFFF));
		return ret;
	}

	WString WString::from(StringView string) {
		WString result;
		result.reserve(string.len());

		for (auto codepoints = string.codepoints(); codepoints; ++codepoints) {
			const Codepoint c = *codepoints;
			result.push(utf32_to_utf16(c));
		}

		return result;
	}

	WString& WString::push(wchar_t w) {
		const auto start_len = m_chars.len();
		
		if (start_len == 0) {
			m_chars.push(w);
			m_chars.push(0);
		} else {
			m_chars.insert(start_len - 1, w);
		}
		
		return *this;
	}

	WString& WString::push(WStringView string) {
		const usize slag = m_chars.cap() - m_chars.len();
		if (slag < string.len()) m_chars.reserve(string.len());

		if (m_chars.len() == 0) m_chars.push(0);
		for (wchar_t w : string) m_chars.insert(m_chars.len() - 1, w);

		return *this;
	}

	WString& WString::push(StringView string) {
		const usize slag = m_chars.cap() - m_chars.len();
		if (slag < string.len()) m_chars.reserve(string.len());

		for (auto codepoints = string.codepoints(); codepoints; ++codepoints) {
			push(utf32_to_utf16(*codepoints));
		}

		return *this;
	}
}