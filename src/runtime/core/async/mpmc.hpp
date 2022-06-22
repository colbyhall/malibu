#pragma once

#include "atomic.hpp"
#include "../memory.hpp"

namespace core::async {
    // Source: Dmitry Vyukov's MPMC
    // http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
    template <typename T>
    class MPMCQueue {
        struct CacheLinePad {
            u8 internal[64];
            CacheLinePad() : internal() {}
        };
        struct Cell {
            Atomic<u32> sequence;
            Option<T> data;
        };

    public:
        static MPMCQueue make(u32 size) {
            // Verify that size is a power of 2
            VERIFY(size >= 2 && (size & size - 1) == 0);

            Cell* buffer = (Cell*)mem::alloc<Cell>(mem::Layout::array<T>(size));
            for (u32 i = 0; i < size; ++i) {
                Cell& cell = buffer[i];
                cell.sequence.store(i, Order::Relaxed);
            }

            return MPMCQueue(buffer, size);
        }

        ~MPMCQueue() {
            if (m_buffer) {
                // FIXME: Free elements
                mem::free(m_buffer);
            }
        }

        bool enqueue(T&& t) {
            Cell* cell = nullptr;
            auto pos = m_enqueue_pos.load();
            for(;;) {
                cell = &m_buffer[pos & m_buffer_mask];
                const auto seq = cell->sequence.load();
                const auto dif = seq - pos;

                if (dif == 0) {
                    if (!m_enqueue_pos.compare_exchange_weak(pos, pos + 1).is_set())
                        break;
                } else if (dif < 0) return false;
                else pos = m_enqueue_pos.load();
            }
            cell->data = core::forward<T>(t);
            cell->sequence.store(pos + 1);
            return true;
        }

        Option<T> dequeue() {
            Cell* cell = nullptr;
            auto pos = m_dequeue_pos.load();
            for (;;) {
                cell = &m_buffer[pos & m_buffer_mask];
                const auto seq = cell->sequence.load();
                const auto dif = seq - (pos + 1);

                if (dif == 0) {
                    if (!m_dequeue_pos.compare_exchange_weak(pos, pos + 1).is_set())
                        break;
                } else if (dif < 0) return Option<T> {};
                else pos = m_dequeue_pos.load();

                T t = cell->data.unwrap();
                cell->sequence.store(pos + m_buffer_mask + 1);
                return t;
            }
        }

    private:
        inline explicit MPMCQueue(Cell* buffer, u32 size) :
            m_buffer(buffer),
            m_buffer_mask(size - 1),
            m_enqueue_pos(0),
            m_dequeue_pos(0) {}

        ALLOW_UNUSED CacheLinePad pad0;
        Cell* m_buffer;
        u32 m_buffer_mask;
        ALLOW_UNUSED CacheLinePad pad1;
        Atomic<u32> m_enqueue_pos;
        ALLOW_UNUSED CacheLinePad pad2;
        Atomic<u32> m_dequeue_pos;
        ALLOW_UNUSED CacheLinePad pad3;
    };
}