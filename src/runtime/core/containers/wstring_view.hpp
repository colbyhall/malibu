#pragma once

#include "string_view.hpp"

namespace core::containers {
	class WStringView {
	public:
		inline constexpr WStringView() : m_chars() {}
		inline constexpr WStringView(Slice<wchar_t const> bytes) : m_chars(bytes) {}
		inline constexpr WStringView(const wchar_t* ptr) : m_chars({ ptr, string_length(ptr) }) {}

		inline operator Slice<wchar_t const>() const { return m_chars; }

		NO_DISCARD inline wchar_t const* ptr() const { return m_chars.ptr(); }
		NO_DISCARD inline usize len() const { return m_chars.len(); }
		inline const wchar_t* operator*() const { return m_chars.ptr(); }

		NO_DISCARD inline const wchar_t* begin() const { return m_chars.cbegin(); }
        NO_DISCARD inline const wchar_t* end() const { return m_chars.cend(); }
        NO_DISCARD inline const wchar_t* cbegin() const { return m_chars.cbegin(); }
		NO_DISCARD ALLOW_UNUSED const wchar_t* cend() const { return m_chars.cend(); }

		NO_DISCARD inline wchar_t operator[](usize index) const { return m_chars[index]; }

	private:
		Slice<wchar_t const> m_chars;
	};
}
using core::containers::WStringView;
