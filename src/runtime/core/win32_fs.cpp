#include "fs.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define WIN32_MEAN_AND_LEAN
#include <windows.h>

namespace core { namespace fs {
	Result<File, FileOpenError> File::open(PathView view, BitFlag<FileFlags> flags) {
		const bool read = flags.is_set(FileFlags::Read);
		const bool write = flags.is_set(FileFlags::Write);
		const bool create = flags.is_set(FileFlags::Create);
		VERIFY(read || write);

		DWORD access = 0;
		if (read) access |= GENERIC_READ;
		if (write) access |= GENERIC_WRITE;

		DWORD creation = OPEN_EXISTING;
		if (create) creation = OPEN_ALWAYS;

		// TODO: UTF8 to UTF16
		// void* handle = CreateFileA();
	}
} }