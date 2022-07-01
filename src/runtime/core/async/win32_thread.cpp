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
        auto& info = *reinterpret_cast<Function<void()>*>(lpParameter);
		info();
		info.~Function();
        mem::free(&info);
        return 0;
    }

    JoinHandle spawn_thread(Function<void()>&& spawn) {
		Function<void()>* param = mem::alloc<Function<void()>>();
		new(param) Function<void()>(forward<Function<void()>>(spawn));

        DWORD id;
        HANDLE handle = CreateThread(
            nullptr,
            0,
            &ThreadProc,
            (LPVOID)param,
            0,
            &id
        );
        VERIFY(handle != nullptr);
        return JoinHandle { handle, id };
    }

    Thread current_thread() {
        return Thread{ GetCurrentThread(), GetCurrentThreadId() };
    }

	u32 logical_core_count() {
		SYSTEM_INFO info = {};
		GetSystemInfo(&info);
		return 0;
	}
}