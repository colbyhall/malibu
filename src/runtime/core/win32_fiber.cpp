#include "fiber.hpp"
#include "memory.hpp"
#include "win32.hpp"

namespace core::fiber {
    Fiber::~Fiber() {
        if (!m_handle) return;

        if (!m_thread) DeleteFiber(m_handle);
        m_handle = nullptr;
    }

    void Fiber::switch_to() {
        SwitchToFiber(m_handle);
    }

    static void WINAPI FiberProc(_In_ LPVOID lpParameter) {
        auto* info = reinterpret_cast<SpawnInfo*>(lpParameter);
        info->proc(info->param);
        mem::free(info);
    }

    Fiber spawn(SpawnInfo info) {
        SpawnInfo* param = mem::alloc<SpawnInfo>();
        *param = info;

        auto* handle = CreateFiber(0, &FiberProc, param);
        return Fiber{ false, handle };
    }

    Fiber convert() {
        auto* handle = ConvertThreadToFiber(nullptr);
        return Fiber{ true, handle };
    }
}