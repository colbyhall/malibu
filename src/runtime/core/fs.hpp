#pragma once

#include "containers/array.hpp"
#include "containers/bitflag.hpp"
#include "containers/result.hpp"
#include "containers/string.hpp"

namespace core::fs {
	class PathView {
	public:
		inline constexpr PathView() : m_string() {}
		inline constexpr PathView(Slice<char const> bytes) : m_string(bytes) {}
		inline constexpr PathView(const char* ptr) : m_string({ ptr, core::containers::string_length(ptr) }) {}

		inline operator StringView() const { return m_string; }
		NO_DISCARD inline char const* ptr() const { return m_string.ptr(); }
		NO_DISCARD inline usize len() const { return m_string.len(); }
		NO_DISCARD inline Chars chars() const { return Chars(m_string); }

	private:
		StringView m_string;
	};

	class Path {
	public:
		inline constexpr Path() : m_string() {}
		explicit inline Path(PathView view) : m_string(view) {}
		inline Path(String&& string) noexcept : m_string(core::forward<String>(string)) {}

		inline operator StringView() const { return m_string; }
		NO_DISCARD inline char const* ptr() const { return m_string.ptr(); }
		NO_DISCARD inline usize len() const { return m_string.len(); }
		NO_DISCARD inline Chars chars() const { return Chars(m_string); }

	private:
		String m_string;
	};

	Path cwd();

	enum class FileFlags : u8 {
		Read,
		Write,
		Create
	};

	enum class FileOpenError : u8 {
		NotFound,
		InUse
	};

	enum class Seek : u8 {
		Begin,
		Current,
		End
	};

	class File {
	public:
		static Result<File, FileOpenError> open(PathView path, BitFlag<FileFlags> flags);

		NO_COPY(File);

		inline File(File&& move) noexcept : m_handle(move.m_handle), m_flags(move.m_flags), m_cursor(0) {
			move.m_handle = nullptr;
		}
		inline File& operator=(File&& move) noexcept {
			File to_destroy = core::move(*this);
			m_handle = move.m_handle;
			m_flags = move.m_flags;
			move.m_handle = nullptr;
			return *this;
		}

		NO_DISCARD usize size() const;
		NO_DISCARD usize cursor() const { return m_cursor; }
		usize seek(Seek method, isize distance);
		inline void rewind() { seek(Seek::Begin, 0); }
		void set_eof();

		usize read(Slice<u8> buffer);
		void write(Slice<const u8> buffer);

		~File();

	private:
		inline File(void* handle, BitFlag<FileFlags> flags) : m_handle(handle), m_flags(flags), m_cursor(0) {}

		void* m_handle;
		BitFlag<FileFlags> m_flags;
		usize m_cursor;
	};

	Result<String, FileOpenError> read_to_string(PathView path);
}