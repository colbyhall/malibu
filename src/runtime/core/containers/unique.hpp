#pragma once

#include "../type_traits.hpp"
#include "../memory.hpp"

#include <new>

namespace core::containers {
	template <typename Base>
	class Unique {
	public:
		template <typename Derived>
		static Unique<Base> make(Derived&& derived) {
			return Unique<Base>(core::forward<Derived>(derived));
		}

		template <typename Derived>
		static Unique<Base> make(const Derived& derived) {
			auto copy = derived;
			return Unique<Base>(core::move(copy));
		}

		NO_COPY(Unique);

		Unique(Unique<Base>&& move) noexcept : m_ptr(move.m_ptr) {
			move.m_ptr = nullptr;
		}
		Unique& operator=(Unique<Base>&& m) noexcept {
			Unique<Base> to_destroy = core::move(*this);
			m_ptr = m.m_ptr;
			m.m_ptr = nullptr;
			return *this;
		}

		~Unique() {
			if (m_ptr) {
				m_ptr->~Base();
				mem::free(m_ptr);
				m_ptr = nullptr;
			}
		}

		NO_DISCARD inline explicit operator NonNull<Base>() { return m_ptr; }
		NO_DISCARD inline explicit operator NonNull<Base const>() const { return m_ptr; }
		NO_DISCARD inline explicit operator Base*() { return m_ptr; }
		NO_DISCARD inline explicit operator Base const *() const { return m_ptr; }
		NO_DISCARD inline Base* operator ->() { return m_ptr; }
		NO_DISCARD inline Base const* operator ->() const { return m_ptr; }
		NO_DISCARD inline Base& operator *() { return *m_ptr; }
		NO_DISCARD inline Base const& operator *() const { return *m_ptr; }

	private:
		Unique() = default;

		template <typename Derived>
		explicit Unique(Derived&& derived) {
			static_assert(core::is_base_of<Base, Derived>, "Base is not a base of Derived");

			void* ptr = mem::alloc(mem::Layout::single<Derived>);
			new (ptr) Derived(core::forward<Derived>(derived)); 
			m_ptr = static_cast<Base*>(ptr);
		}

		Base* m_ptr;
	};
}
using core::containers::Unique;
