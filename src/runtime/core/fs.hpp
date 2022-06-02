#pragma once

#include "containers/array.hpp"
#include "containers/bitflag.hpp"
#include "containers/result.hpp"

namespace core { namespace fs {
	class Path {
	public:
		ALWAYS_INLINE constexpr Path() = default;

	private:
		Array<u8> m_buffer;
	};

	class PathView {
	public:
		ALWAYS_INLINE constexpr PathView() = default;

	private:
		Slice<const char> m_buffer;
	};

	enum class FileFlags : u8 {
		Read,
		Write,
		Create
	};

	enum class FileOpenError : u8 {
		NotFound,
		InUse
	};

	class File {
	public:
		// TODO: Error handling with core::containers::Result
		static Result<File, FileOpenError> open(PathView path, BitFlag<FileFlags> flags);

		NO_COPY(File);

	private:
		ALWAYS_INLINE File(void* handle, BitFlag<FileFlags> flags) : m_handle(handle), m_flags(flags) {}

		void* m_handle;
		BitFlag<FileFlags> m_flags;
	};
} }