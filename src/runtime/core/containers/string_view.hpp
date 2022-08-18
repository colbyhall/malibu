#pragma once

#include "slice.hpp"
#include "hash.hpp"

namespace core::containers {
	template<typename T>
	constexpr usize string_length(const T* string) {
		if (*string == 0) return 0;
		string += 1;
		return 1 + string_length(string);
	}

	using Codepoint = u32;
	constexpr Codepoint EOS = 0;
	constexpr Codepoint EOL = '\n';
	constexpr Codepoint UTF8_BOM = 0xfeff;

	class CodepointsIterator {
	public:
		inline explicit CodepointsIterator(Slice<char const> string) : m_string(string), m_index(0), m_decoder_state(0), m_codepoint(0) {}

		inline explicit operator bool() const { return should_continue(); }
		inline CodepointsIterator& operator++() { next(); return *this; }
		inline Codepoint operator*() const { return get(); }

	private:
		bool should_continue() const;
		void next();
		Codepoint get() const;

		Slice<char const> m_string;
		usize m_index;
		u32 m_decoder_state;
		Codepoint m_codepoint;
	};

	class StringView {
	public:
		inline constexpr StringView() : m_bytes() {}
		inline constexpr StringView(Slice<char const> bytes) : m_bytes(bytes) {}
		inline constexpr StringView(const char* ptr) : m_bytes({ ptr, string_length(ptr) }) {}

		inline operator Slice<char const>() const { return m_bytes; }
		NO_DISCARD inline char const* ptr() const { return m_bytes.ptr(); }
		NO_DISCARD inline usize len() const { return m_bytes.len(); }
		NO_DISCARD inline CodepointsIterator codepoints() const { return CodepointsIterator(m_bytes); }
		inline const char* operator*() const { return m_bytes.ptr(); }

		inline bool operator==(const StringView& right) const {
			if (len() != right.len()) return false;

			for (usize i = 0; i < len(); i++) {
				if (m_bytes[i] != right.m_bytes[i]) return false;
			}

			return true;
		}
		inline bool operator!=(const StringView& right) const {
			return !(*this == right);
		}

	private:
		Slice<char const> m_bytes;
	};

	inline u64 hash(StringView view) {
		return hash::fnv1_hash(Slice<u8 const>((u8 const*)view.ptr(), view.len()));
	}
}
using core::containers::Codepoint;
using core::containers::CodepointsIterator;
using core::containers::StringView;