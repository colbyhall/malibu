#pragma once

#include "type_traits.hpp"

namespace core::fiber {
    class Fiber {
    public:
        using Handle = void*;

        NO_COPY(Fiber);
        inline Fiber(Fiber&& move) noexcept
                : m_thread(move.m_thread), m_handle(move.m_handle){
            move.m_handle = nullptr;
        }
        inline Fiber& operator=(Fiber&& move) noexcept {
            // FIXME: Is this the best way to do this
            Fiber to_destroy = core::move(*this);
            m_handle = move.m_handle;
            m_thread = move.m_thread;
            move.m_handle = nullptr;
            return *this;
        }
        ~Fiber();

        void switch_to();

        NO_DISCARD inline Handle handle() const { return m_handle; }

    private:
        Fiber(bool thread, Handle handle) : m_thread(thread), m_handle(handle) {}

        friend Fiber spawn(struct SpawnInfo info);
        friend Fiber convert();

        bool m_thread;
        Handle m_handle;
    };

    struct SpawnInfo {
        void (*proc)(void* param) = nullptr;
        void* param = nullptr;

        SpawnInfo() = default;
        inline SpawnInfo(void (*_proc)(void* param)) : proc(_proc) {}
        inline explicit SpawnInfo(void (*_proc)(void* param), void* _param) : proc(_proc), param(_param) {}
    };

    Fiber spawn(SpawnInfo info);

    // Converts current thread into fiber
    Fiber convert();

}