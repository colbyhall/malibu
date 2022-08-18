#include "string_view.hpp"

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

	bool CodepointsIterator::should_continue() const {
		return m_string.len() > 0 && m_index < m_string.len() && m_decoder_state != utf8_reject;
	}

	void CodepointsIterator::next() {
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

	Codepoint CodepointsIterator::get() const {
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
}