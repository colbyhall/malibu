#include "thread.hpp"
#include "../memory.hpp"
#include "../win32.hpp"

#include <new>

namespace core::async {
	bool JoinHandle::join() {
		// TODO: Check the result so we can tell the user if the thread had already ended
		WaitForSingleObject(
				(HANDLE)m_thread.handle(),
				INFINITE
		);

		m_joined = true;
		return true;
	}

	JoinHandle::~JoinHandle() {
		if (!m_joined) {
			join();
		}
	}

	static DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter) {
		auto& info = *reinterpret_cast<Thread::Function*>(lpParameter);
		info();
		info.~Function();
		mem::free(&info);
		return 0;
	}

	JoinHandle Thread::spawn(bool start, Thread::Function&& spawn) {
		Thread::Function* param = mem::alloc<Thread::Function>();
		new(param) Thread::Function(forward<Thread::Function>(spawn));

		auto flags = 0;
		if (!start) {
			flags = CREATE_SUSPENDED;
		}

		DWORD id;
		HANDLE handle = CreateThread(
			nullptr,
			0,
			&ThreadProc,
			(LPVOID)param,
			flags,
			&id
		);
		VERIFY(handle != nullptr);
		return JoinHandle { handle, id };
	}

	Thread Thread::current() {
		return Thread{ GetCurrentThread(), GetCurrentThreadId() };
	}

	void Thread::resume() const {
		ResumeThread(m_handle);
	}

	u32 logical_core_count() {
		SYSTEM_INFO info = {};
		GetSystemInfo(&info);
		return (u32)info.dwNumberOfProcessors;
	}
}