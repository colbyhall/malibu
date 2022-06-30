#include "thread.hpp"
#include "../memory.hpp"
#include "../win32.hpp"

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
        auto* info = reinterpret_cast<ThreadSpawnInfo*>(lpParameter);
        info->proc(info->param);
        mem::free(info);
        return 0;
    }

    JoinHandle spawn_thread(ThreadSpawnInfo info) {
		ThreadSpawnInfo* param = mem::alloc<ThreadSpawnInfo>();
        *param = info;

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