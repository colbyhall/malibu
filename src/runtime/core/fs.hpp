#pragma once

#include "containers/array.hpp"
#include "containers/bitflag.hpp"
#include "containers/result.hpp"
#include "containers/string.hpp"

namespace core { namespace fs {
	class PathView {
	public:
		ALWAYS_INLINE constexpr PathView() : m_string() {}
		ALWAYS_INLINE constexpr PathView(Slice<char const> bytes) : m_string(bytes) {}
		ALWAYS_INLINE constexpr PathView(const char* ptr) : m_string({ ptr, core::containers::string_length(ptr) }) {}

		ALWAYS_INLINE operator StringView() const { return m_string; }
		NO_DISCARD ALWAYS_INLINE char const* ptr() const { return m_string.ptr(); }
		NO_DISCARD ALWAYS_INLINE usize len() const { return m_string.len(); }
		NO_DISCARD ALWAYS_INLINE Chars chars() const { return Chars(m_string); }

	private:
		StringView m_string;
	};

	class Path {
	public:
		ALWAYS_INLINE constexpr Path() : m_string() {}
		explicit ALWAYS_INLINE Path(PathView view) : m_string(view) {}
		ALWAYS_INLINE Path(String&& string) noexcept : m_string(core::forward<String>(string)) {}

		ALWAYS_INLINE operator StringView() const { return m_string; }
		NO_DISCARD ALWAYS_INLINE char const* ptr() const { return m_string.ptr(); }
		NO_DISCARD ALWAYS_INLINE usize len() const { return m_string.len(); }
		NO_DISCARD ALWAYS_INLINE Chars chars() const { return Chars(m_string); }

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
		// TODO: Error handling with core::containers::Result
		static Result<File, FileOpenError> open(PathView path, BitFlag<FileFlags> flags);

		NO_COPY(File);

		ALWAYS_INLINE File(File&& move) noexcept : m_handle(move.m_handle), m_flags(move.m_flags), m_cursor(0) {
			move.m_handle = nullptr;
		}
		ALWAYS_INLINE File& operator=(File&& move) noexcept {
			File to_destroy = core::move(*this);
			m_handle = move.m_handle;
			m_flags = move.m_flags;
			move.m_handle = nullptr;
			return *this;
		}

		NO_DISCARD usize size() const;
		NO_DISCARD usize cursor() const { return m_cursor; }
		usize seek(Seek method, isize distance);
		ALWAYS_INLINE void rewind() { seek(Seek::Begin, 0); }
		void set_eof();

		usize read(Slice<u8> buffer);
		void write(Slice<const u8> buffer);

		~File();

	private:
		ALWAYS_INLINE File(void* handle, BitFlag<FileFlags> flags) : m_handle(handle), m_flags(flags), m_cursor(0) {}

		void* m_handle;
		BitFlag<FileFlags> m_flags;
		usize m_cursor;
	};

} }