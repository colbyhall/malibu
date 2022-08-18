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

	String String::from(WStringView string) {
		// FIXME: Do proper utf16 decode
		String ret;
		ret.reserve(string.len());
		for (wchar_t c : string) ret.push((Codepoint)c);
		return ret;
	}

	String& String::push(Codepoint c) {
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
}