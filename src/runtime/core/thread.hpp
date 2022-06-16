#pragma once

#include "type_traits.hpp"

#include <functional>

namespace core::thread {
    class JoinHandle;

    class Thread {
    public:
        using Id = u32;
        using Handle = void*;

        NO_DISCARD inline Handle handle() const { return m_handle; }
        NO_DISCARD inline Id id() const { return m_id; }

    private:
        inline explicit Thread(void* handle, Id id) : m_handle(handle), m_id(id) {}
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
        NO_DISCARD inline const Thread& thread() const { return m_thread; }

        ~JoinHandle();
    private:
        inline explicit JoinHandle(void* handle, Thread::Id id) : m_thread(handle, id), m_joined(false) {}

        friend JoinHandle spawn(struct SpawnInfo info);

        Thread m_thread;
        bool m_joined;
    };


    struct SpawnInfo {
        void (*proc)(void* param) = nullptr;
        void* param = nullptr;

        SpawnInfo() = default;
        inline SpawnInfo(void (*_proc)(void* param)) : proc(_proc) {}
        inline explicit SpawnInfo(void (*_proc)(void* param), void* _param) : proc(_proc), param(_param) {}
    };
    ALLOW_UNUSED JoinHandle spawn(SpawnInfo info);

    ALLOW_UNUSED Thread current();
}