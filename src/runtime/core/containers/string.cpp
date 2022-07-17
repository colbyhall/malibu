#include "string.hpp"

namespace core::containers {
	static const u8 utf8d[] = {
		// The first part of the table maps bytes to character classes that
		// to reduce the size of the transition table and create bitmasks.
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
		7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

		// The second part is a transition table that maps a combination
		// of a state of the automaton and a character class to a state.
		0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
		12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
		12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
		12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
		12,36,12,12,12,12,12,12,12,12,12,12,
	};

	constexpr u32 utf8_accept = 0;
	constexpr u32 utf8_reject = 12;

	static u32 utf8_decode(u32* state, u32* code_p, u32 byte) {
		u32 type = utf8d[byte];

		*code_p = (*state != utf8_accept) ? (byte & 0x3fu) | (*code_p << 6) : (0xff >> type) & (byte);

		*state = utf8d[256 + *state + type];
		return *state;
	}

	static i32 utf8_encode(u32 c, void* dest, u32* errors) {
		*errors |= ((c << 11) == 0x1b) | (c > 0x10ffff) << 1 | ((c >> 1) == 0x7fff) << 2;
		char len = (c > 0x7f) + (c > 0x7ff) + (c > 0xffff);
		char* p = (char*)dest;

		char head = 0xf0e0c000 >> (len << 3);
		p[len] = 0x80 | c & 0x3f;
		p[len >> 1] = 0x80 | c >> 12 & 0x3f;
		p[1 << len >> 2] = 0x80 | c >> 6 & 0x3f;
		p[0] = head | c >> len * 6 & ~head >> 1;
		return len + 1;
	}

	bool CharsIterator::should_continue() const {
		return m_string.len() > 0 && m_index < m_string.len() && m_decoder_state != utf8_reject;
	}

	void CharsIterator::next() {
		VERIFY(should_continue());

		for (; m_index < m_string.len(); m_index += 1) {
			const u8 c = m_string[m_index];
			utf8_decode(&m_decoder_state, &m_codepoint, c);

			if (m_decoder_state == utf8_reject) return;
			if (m_decoder_state != utf8_accept) continue;

			break;
		}

		m_index += 1;
	}

	Char CharsIterator::get() const {
		usize get_index = m_index;
		u32 get_state = m_decoder_state;
		u32 get_codepoint = m_codepoint;
		for (; get_index < m_string.len(); get_index += 1) {
			const u8 c = (u8)m_string[get_index];
			utf8_decode(&get_state, &get_codepoint, c);

			if (get_state == utf8_reject) return 0xfffd;
			if (get_state != utf8_accept) continue;

			break;
		}
		return get_codepoint;
	}

	String String::from(WStringView string) {
		// FIXME: Do proper utf16 decode
		String ret;
		ret.reserve(string.len());
		for (wchar_t c : string) ret.push((Char)c);
		return ret;
	}

	String& String::push(Char c) {
		// Encode the utf32 character to an utf8 multi width character
		u8 local[4] = {};
		u32 error;
		const usize char_len = utf8_encode(c, local, &error);
		VERIFY(error != utf8_reject);
		
		// Preallocate enough space to add the bytes
		const usize slag = m_bytes.cap() - m_bytes.len();
		if (slag < char_len) m_bytes.reserve(char_len + 1);

		// Add the null terminator if the string len was 0
		if (m_bytes.len() == 0) m_bytes.push(0);

		// SPEED: Could be made faster if used mem::copy
		for (usize i = 0; i < char_len; ++i) m_bytes.insert(m_bytes.len() - 1, local[i]);

		return *this;
	}

	String& String::push(StringView string) {
		// Preallocate enough space for the entire string to reduce allocations
		const usize slag = m_bytes.cap() - m_bytes.len();
		if (slag < string.len()) m_bytes.reserve(string.len());

		// Add the null terminator if the string len was 0
		if (m_bytes.len() == 0) m_bytes.push(0);

		// SPEED: could be made faster if used mem::copy
		for (int i = 0; i < string.len(); ++i) m_bytes.insert(m_bytes.len() - 1, string.ptr()[i]);

		return *this;
	}

	static wchar_t utf32_to_utf16(Char c) {
		u32 h;
		u32 l;

		if (c < 0x10000) {
			h = 0;
			l = c;
			return c;
		}
		Char t = c - 0x10000;
		h = (((t<<12)>>22) + 0xD800);
		l = (((t<<22)>>22) + 0xDC00);
		Char ret = ((h<<16) | ( l & 0x0000FFFF));
		return ret;
	}

	WString WString::from(StringView string) {
		WString result;
		result.reserve(string.len());

		for (auto chars = string.chars(); chars; ++chars) {
			const Char c = *chars;
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

		for (auto chars = string.chars(); chars; ++chars) {
			push(utf32_to_utf16(*chars));
		}

		return *this;
	}
}