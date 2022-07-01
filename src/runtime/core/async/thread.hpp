#pragma once

#include "../containers/function.hpp"

namespace core::async {
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

        friend Thread current_thread();
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

        friend JoinHandle spawn_thread(Function<void()>&&);

        Thread m_thread;
        bool m_joined;
    };


    ALLOW_UNUSED JoinHandle spawn_thread(Function<void()>&& spawn);
    ALLOW_UNUSED Thread current_thread();

	NO_DISCARD u32 logical_core_count();
}