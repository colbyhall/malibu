#pragma once

#include "string_view.hpp"
#include "wstring_view.hpp"
#include "array.hpp"

namespace core::containers {
	class String {
	public:
		inline constexpr String() : m_bytes() {}
		inline static String from(Array<char>&& bytes) {
			String string;
			string.m_bytes = core::forward<Array<char>>(bytes);
			if (string.m_bytes.len() > 0 && string.m_bytes[string.m_bytes.len() - 1] != 0) {
				string.m_bytes.push(0);
			}
			return string;
		}

		static inline String from(StringView view) {
			String string;
			string.m_bytes = Array<char>(view);
			string.m_bytes.push(0);
			return string;
		}
		static String from(WStringView string);

		inline operator Slice<char const>() const {
			Slice<char const> result = m_bytes;
			if (m_bytes.len() > 0) {
				result = result.shrink(1);
			}
			return result; 
		}
		inline operator StringView() const { 
			Slice<char const> bytes = m_bytes;
			bytes.set_len(len());
			return StringView(bytes); 
		}

		NO_DISCARD inline char* ptr() { return m_bytes.ptr(); }
		NO_DISCARD inline char const* ptr() const { return m_bytes.ptr(); }
		inline const char* operator*() const { return m_bytes.ptr(); }

		NO_DISCARD inline usize len() const { return m_bytes.len() > 0 ? m_bytes.len() - 1 : 0; }
		NO_DISCARD inline usize cap() const { return m_bytes.cap(); }
		inline void set_len(usize len) { 
			m_bytes.set_len(len + 1);
			m_bytes[len] = 0;
		}

		NO_DISCARD inline CodepointsIterator codepoints() const { return CodepointsIterator(m_bytes); }

		inline void reserve(usize amount) { m_bytes.reserve(amount); }
		String& push(Codepoint c);
		String& push(StringView string);
		
	private:
		Array<char> m_bytes;
	};
}
using core::containers::String;