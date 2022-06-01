#pragma once

#include "../type_traits.hpp"
#include "../memory.hpp"

#include <new>

namespace core { namespace containers {
	template <typename Base>
	class SharedRef {
		struct Counter {
			usize strong_count; // TODO: Atomics
			usize weak_count; // TODO: Atomics
		};
	public:
		template <typename Derived>
		static SharedRef<Base> make(Derived&& derived) {
			return SharedRef<Base>(core::forward<Derived>(derived));
		}

		template <typename Derived>
		static SharedRef<Base> make(const Derived& derived) {
			auto copy = derived;
			return SharedRef<Base>(core::move(copy));
		}

		NO_COPY(SharedRef);

		SharedRef(SharedRef<Base>&& m) noexcept : m_counter(m.m_counter), m_ptr(m.m_ptr) {
			m.m_ptr = nullptr;
			m.m_counter = nullptr;
		}
		SharedRef<Base>& operator=(SharedRef<Base>&& m) noexcept {
			SharedRef<Base> to_destroy = core::move(*this);
			m_counter = m.m_counter;
			m_ptr = m.m_ptr;

			m.m_ptr = nullptr;
			m.m_counter = nullptr;

			return *this;
		}

		~SharedRef() {
			if (m_counter) {
				m_counter->strong_count -= 1;

				if (m_counter->strong_count == 0) {
					mem::free(m_counter);
					m_counter = nullptr;

					m_ptr->~Base();
					mem::free(m_ptr);
					m_ptr = nullptr;
				}
			}
		}

		NO_DISCARD ALWAYS_INLINE explicit operator Base const *() const { return m_ptr; }
		NO_DISCARD ALWAYS_INLINE Base const* operator ->() const { return m_ptr; }
		NO_DISCARD ALWAYS_INLINE Base const& operator *() const { return *m_ptr; }

	private:
		SharedRef() = default;

		template <typename Derived>
		explicit SharedRef(Derived&& derived) { 
			static_assert(core::is_base_of<Base, Derived> || core::is_same<Base, Derived>, "Base is not a base of Derived");
			static_assert(!core::is_abstract<Derived>, "Derived must not be abstract");

			void* counter = mem::alloc(mem::Layout::single<Counter>);
			m_counter = new (counter) Counter { 1, 0 };

			void* ptr = mem::alloc(mem::Layout::single<Derived>);
			m_ptr = new (ptr) Derived(core::move(derived));
		}

		Counter* m_counter;
		Base* m_ptr;
	};
} }

template <typename T>
using SharedRef = core::containers::SharedRef<T>;