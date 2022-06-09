#pragma once

#include "fs.hpp"

namespace core::library {
	class Library {
	public:
		Library() : m_handle(nullptr) { }
		static Option<Library> open(fs::PathView path);
		~Library();

		NO_COPY(Library);

		ALWAYS_INLINE Library(Library&& move) noexcept : m_handle(move.m_handle) {
			move.m_handle = nullptr;
		}
		ALWAYS_INLINE Library& operator=(Library&& move) noexcept {
			// FIXME: Is this the best way to do this
			Library to_destroy = core::move(*this);
			m_handle = move.m_handle;
			move.m_handle = nullptr;
			return *this;
		}

		void* find(StringView name);

	private:
		ALWAYS_INLINE Library(void* handle) : m_handle(handle) { }

		void* m_handle;
	};
}