#pragma once

#include "../templates/type_traits.hpp"
#include "../memory.hpp"
#include "../async/atomic.hpp"

#include <new>

namespace core::containers {
	struct Counter {
		usize strong_count;
	};

	template <typename Base>
	class SharedRef {
	public:

		template<typename Derived = Base, typename... Args>
		static SharedRef<Base> make(Args&&... args) {
			return SharedRef<Base>(Derived(forward<Args>(args)...));
		}

		template <typename Derived>
		static SharedRef<Base> make(const Derived& derived) {
			auto copy = derived;
			return SharedRef<Base>(core::move(copy));
		}

		template <typename Derived>
		SharedRef(SharedRef<Derived>&& derived) {
			static_assert(core::is_base_of<Base, Derived> || core::is_same<Base, Derived>, "Base is not a base of Derived");
			static_assert(!core::is_abstract<Derived>, "Derived must not be abstract");

			m_counter = derived.m_counter;
			m_ptr = derived.m_ptr;
			derived.m_counter = nullptr;
			derived.m_ptr = nullptr;
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

		SharedRef clone() const {
			VERIFY(m_counter && m_ptr);
			m_counter->strong_count += 1;
			return SharedRef{ m_counter, m_ptr };
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

		NO_DISCARD inline explicit operator Base const* () const { return m_ptr; }
		NO_DISCARD inline Base* operator ->() { return m_ptr; }
		NO_DISCARD inline Base& operator *() { return *m_ptr; }
		NO_DISCARD inline Base const* operator ->() const { return m_ptr; }
		NO_DISCARD inline Base const& operator *() const { return *m_ptr; }

		NO_DISCARD inline Base* ptr() { return m_ptr; }
		NO_DISCARD inline Base const* ptr() const { return m_ptr; }

		// NO_DISCARD inline Counter* counter() { return m_counter; }
		// NO_DISCARD inline Counter const* counter() const { return m_counter; }

	private:
		SharedRef() = default;
		SharedRef(Counter* counter, Base* ptr) : m_counter(counter), m_ptr(ptr) {}

		template<typename T>
		friend class SharedRef;

		template <typename Derived>
		explicit SharedRef(Derived&& derived) {
			static_assert(core::is_base_of<Base, Derived> || core::is_same<Base, Derived>, "Base is not a base of Derived");
			static_assert(!core::is_abstract<Derived>, "Derived must not be abstract");

			void* counter = mem::alloc(mem::Layout::single<Counter>);
			m_counter = new (counter) Counter{ 1 };

			void* ptr = mem::alloc(mem::Layout::single<Derived>);
			m_ptr = new (ptr) Derived(core::forward<Derived>(derived));
		}

		Counter* m_counter;
		Base* m_ptr;
	};

	template <typename Base>
	class AtomicSharedRef {
		struct Counter {
			async::Atomic<usize> strong_count;
		};
	public:
		template<typename Derived = Base, typename... Args>
		static AtomicSharedRef<Base> make(Args&&... args) {
			return AtomicSharedRef<Base>(Derived(forward<Args>(args)...));
		}

		template <typename Derived>
		static AtomicSharedRef<Base> make(const Derived& derived) {
			auto copy = derived;
			return AtomicSharedRef<Base>(core::move(copy));
		}

		NO_COPY(AtomicSharedRef);

		AtomicSharedRef(AtomicSharedRef<Base>&& m) noexcept : m_counter(m.m_counter), m_ptr(m.m_ptr) {
			m.m_ptr = nullptr;
			m.m_counter = nullptr;
		}
		AtomicSharedRef<Base>& operator=(AtomicSharedRef<Base>&& m) noexcept {
			AtomicSharedRef<Base> to_destroy = core::move(*this);
			m_counter = m.m_counter;
			m_ptr = m.m_ptr;

			m.m_ptr = nullptr;
			m.m_counter = nullptr;

			return *this;
		}

		AtomicSharedRef clone() const {
			VERIFY(m_counter && m_ptr);
			m_counter->strong_count.fetch_add(1);
			return AtomicSharedRef{ m_counter, m_ptr };
		}

		~AtomicSharedRef() {
			if (m_counter) {
				const auto strong = m_counter->strong_count.fetch_sub(1);

				if (strong == 1) {
					mem::free(m_counter);
					m_counter = nullptr;

					m_ptr->~Base();
					mem::free(m_ptr);
					m_ptr = nullptr;
				}
			}
		}

		NO_DISCARD inline explicit operator Base const *() const { return m_ptr; }
		NO_DISCARD inline Base const* operator ->() const { return m_ptr; }
		NO_DISCARD inline Base const& operator *() const { return *m_ptr; }

	private:
		AtomicSharedRef() = default;
		AtomicSharedRef(Counter* counter, Base* ptr) : m_counter(counter), m_ptr(ptr) {}

		template <typename Derived>
		explicit AtomicSharedRef(Derived&& derived) { 
			static_assert(core::is_base_of<Base, Derived> || core::is_same<Base, Derived>, "Base is not a base of Derived");
			static_assert(!core::is_abstract<Derived>, "Derived must not be abstract");

			void* counter = mem::alloc(mem::Layout::single<Counter>);
			m_counter = new (counter) Counter { 1 };

			void* ptr = mem::alloc(mem::Layout::single<Derived>);
			m_ptr = new (ptr) Derived(core::forward<Derived>(derived));
		}

		Counter* m_counter;
		Base* m_ptr;
	};
}
using core::containers::SharedRef;
using core::containers::AtomicSharedRef;