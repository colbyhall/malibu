#pragma once

#include "../primitives.hpp"
#include "../containers/option.hpp"
#include <atomic>

namespace core::async {
    enum class Order : u8 {
        Relaxed,
        SequentiallyConsistent
    };

    template <typename T>
    class Atomic {
    public:
        Atomic() noexcept = default;
        constexpr Atomic(T desired) noexcept : m_atomic(desired) {}

        inline void store(T desired, Order order = Order::SequentiallyConsistent) noexcept {
            m_atomic.store(desired, to_std(order));
        }

        NO_DISCARD inline T load(Order order = Order::SequentiallyConsistent) const noexcept {
            return m_atomic.load(to_std(order));
        }

        NO_DISCARD inline T exchange(T desired, Order order = Order::SequentiallyConsistent) noexcept {
            return m_atomic.exchange(desired, to_std(order));
        }

        NO_DISCARD inline Option<T> compare_exchange_weak(
            T expected,
            T desired,
            Order order = Order::SequentiallyConsistent
        ) noexcept {
            if (!m_atomic.compare_exchange_weak(expected, desired, to_std(order))) {
                return expected;
            }
            return Option<T> {};
        }

        NO_DISCARD inline Option<T> compare_exchange_strong(
            T expected,
            T desired,
            Order order = Order::SequentiallyConsistent
        ) noexcept {
            if (!m_atomic.compare_exchange_strong(expected, desired, to_std(order))) {
                return expected;
            }
            return Option<T> {};
        }

        inline T fetch_add(T arg, Order order = Order::SequentiallyConsistent) noexcept {
            return m_atomic.fetch_add(arg, to_std(order));
        }

        inline T fetch_sub(T arg, Order order = Order::SequentiallyConsistent) noexcept {
            return m_atomic.fetch_sub(arg, to_std(order));
        }

        inline T fetch_and(T arg, Order order = Order::SequentiallyConsistent) noexcept {
            return m_atomic.fetch_and(arg, to_std(order));
        }

        inline T fetch_or(T arg, Order order = Order::SequentiallyConsistent) noexcept {
            return m_atomic.fetch_or(arg, to_std(order));
        }

        inline T fetch_xor(T arg, Order order = Order::SequentiallyConsistent) noexcept {
            return m_atomic.fetch_xor(arg, to_std(order));
        }

    private:
        inline std::memory_order to_std(Order order) {
            static const std::memory_order convert[] = {
                    std::memory_order_relaxed,
                    std::memory_order_seq_cst
            };
            return convert[(u8)order];
        }

        std::atomic<T> m_atomic;
    };
}