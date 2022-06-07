#include "fs.hpp"

#define WIN32_LEAN_AND_MEAN
#define WIN32_MEAN_AND_LEAN
#include <windows.h>

namespace core::fs {
	Path cwd() {
		// Query the length of the path
		const auto len = (usize)GetCurrentDirectoryW(0, nullptr);
		VERIFY(len > 0);

		// Create a wide string buffer to get the cwd path
		WString buffer;
		buffer.reserve(len);
		GetCurrentDirectoryW((DWORD)buffer.cap(), buffer.ptr());
		buffer.set_len(len);

		// Copy all data to String
		return {String::from(buffer) };
	}

	Result<File, FileOpenError> File::open(PathView path, BitFlag<FileFlags> flags) {
		const bool read = flags.is_set(FileFlags::Read);
		const bool write = flags.is_set(FileFlags::Write);
		const bool create = flags.is_set(FileFlags::Create);
		VERIFY(read || write);

		DWORD access = 0;
		if (read) access |= GENERIC_READ;
		if (write) access |= GENERIC_WRITE;

		DWORD creation = OPEN_EXISTING;
		if (create) creation = OPEN_ALWAYS;

		WString wpath;
		wpath.reserve(path.len() + 16);
		// TODO: Prepend this to allow paths past MAX_PATH
		// path.push(L"\\\\?\\");
		wpath.push(path);

		void* handle = CreateFileW(
			wpath.ptr(),
			access,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			0,
			creation,
			FILE_ATTRIBUTE_NORMAL,
			0
		);

		if (handle == INVALID_HANDLE_VALUE) {
			const DWORD err = GetLastError();
			TODO("File open error handling");
		}

		return File{ handle, flags };
	}

	usize File::size() const {
		DWORD high;
		const DWORD low = GetFileSize(m_handle, &high);
		return (static_cast<usize>(high) << 32) | static_cast<usize>(low);
	}

	usize File::seek(Seek method, isize distance) {
		auto win32_method = static_cast<DWORD>(method);
		
		LARGE_INTEGER win32_distance;
		win32_distance.QuadPart = distance;
		LARGE_INTEGER new_cursor;
		const bool ok = SetFilePointerEx(
			m_handle, 
			win32_distance, 
			&new_cursor, 
			win32_method
		);
		VERIFY(ok);

		m_cursor = static_cast<usize>(new_cursor.QuadPart);
		return m_cursor;
	}

	void File::set_eof() {
		VERIFY(m_flags.is_set(FileFlags::Write));
		const bool ok = SetEndOfFile(m_handle);
		VERIFY(ok);
	}

	usize File::read(Slice<u8> buffer) {
		VERIFY(m_flags.is_set(FileFlags::Read));

		DWORD amount_read;
		const bool ok = ReadFile(
			m_handle, 
			buffer.ptr(), 
			(DWORD)buffer.len(), 
			&amount_read, 
			nullptr
		);
		VERIFY(ok);

		m_cursor += amount_read;

		return amount_read;
	}

	void File::write(Slice<const u8> buffer) {
		VERIFY(m_flags.is_set(FileFlags::Write));

		const bool ok = WriteFile(
			m_handle, 
			buffer.ptr(), 
			(DWORD)buffer.len(), 
			nullptr, 
			nullptr
		);
		VERIFY(ok);

		m_cursor += buffer.len();
	}

	File::~File() {
		if (m_handle) {
			const bool ok = CloseHandle((HANDLE)m_handle) > 0;
			VERIFY(ok);
			m_handle = nullptr;
		}
	}
}