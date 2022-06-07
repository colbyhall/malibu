#include "library.hpp"

#define WIN32_LEAN_AND_MEAN
#define WIN32_MEAN_AND_LEAN
#include <windows.h>

namespace core::library {
	Option<Library> Library::open(fs::PathView path) {
		WString wpath;
		wpath.reserve(path.len());
		wpath.push(path);

		void* handle = LoadLibraryW(wpath.ptr());
		if (handle != nullptr) return Library { handle };
		return Option<Library>();
	}

	Library::~Library() {
		if (m_handle) {
			FreeLibrary((HMODULE)m_handle);
			m_handle = nullptr;
		}
	}

	void* Library::find(StringView name) {
		return (void*)GetProcAddress((HMODULE)m_handle, name.ptr());
	}
}