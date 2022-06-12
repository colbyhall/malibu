#pragma once

#include "type_traits.hpp"

#include <functional>

namespace core::thread {
    class JoinHandle;

    class Thread {
    public:
        using Id = u32;
        using Handle = void*;

        NO_DISCARD ALWAYS_INLINE Handle handle() const { return m_handle; }
        NO_DISCARD ALWAYS_INLINE Id id() const { return m_id; }

    private:
        ALWAYS_INLINE explicit Thread(void* handle, Id id) : m_handle(handle), m_id(id) {}
        NO_COPY(Thread);
        NO_MOVE(Thread);

        friend Thread current();
        friend JoinHandle;

        Handle m_handle;
        Id m_id;
    };

    class JoinHandle {
    public:
        bool join();
        NO_DISCARD ALWAYS_INLINE const Thread& thread() const { return m_thread; }

        ~JoinHandle();
    private:
        ALWAYS_INLINE explicit JoinHandle(void* handle, Thread::Id id) : m_thread(handle, id), m_joined(false) {}

        friend JoinHandle spawn(struct SpawnInfo info);

        Thread m_thread;
        bool m_joined;
    };


    struct SpawnInfo {
        void (*proc)(void* param) = nullptr;
        void* param = nullptr;

        SpawnInfo() = default;
        ALWAYS_INLINE SpawnInfo(void (*_proc)(void* param)) : proc(_proc) {}
        ALWAYS_INLINE explicit SpawnInfo(void (*_proc)(void* param), void* _param) : proc(_proc), param(_param) {}
    };
    ALLOW_UNUSED JoinHandle spawn(SpawnInfo info);

    ALLOW_UNUSED Thread current();
}