#include "thread.hpp"
#include "memory.hpp"

#define WIN32_LEAN_AND_MEAN
#define WIN32_MEAN_AND_LEAN
#include <windows.h>

namespace core::thread {
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
        auto* info = reinterpret_cast<SpawnInfo*>(lpParameter);
        info->proc(info->param);
        mem::free(info);
        return 0;
    }

    JoinHandle spawn(SpawnInfo info) {
        SpawnInfo* param = mem::alloc<SpawnInfo>();
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

    Thread current() {
        return Thread{ GetCurrentThread(), GetCurrentThreadId() };
    }
}