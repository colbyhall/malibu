#pragma once

#include "../type_traits.hpp"
#include "../memory.hpp"

#include <new>

namespace core { namespace containers {
	template <typename Base>
	class Unique {
	public:
		template <typename Derived>
		explicit Unique(Derived&& t) {
			static_assert(core::is_base_of<Base, Derived>, "Base is not a base of Derived");

			void* ptr = mem::alloc(mem::Layout::single<Derived>);
			new (ptr) Derived(core::move(t)); 
			m_ptr = static_cast<Base*>(ptr);
		}

		template <typename Derived>
		explicit Unique(const Derived& t) {
			static_assert(core::is_base_of<Base, Derived>, "Base is not a base of Derived");
			Derived copy = t;
			void* ptr = mem::alloc(mem::Layout::single<Derived>);
			new (ptr) Derived(core::move(copy));
			m_ptr = static_cast<Base*>(ptr);
		}

		template<typename Derived>
		Unique(Unique<Derived>&& move) noexcept : m_ptr(move.m_ptr) {
			static_assert(core::is_base_of<Base, Derived>, "Base is not a base of Derived");
			move.m_ptr = nullptr;
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

		NO_DISCARD ALWAYS_INLINE explicit
		operator NonNull<Base>() { return m_ptr; }

		NO_DISCARD ALWAYS_INLINE explicit
		operator NonNull<Base const>() const { return m_ptr; }

		NO_DISCARD ALWAYS_INLINE explicit
		operator Base*() { return m_ptr; }

		NO_DISCARD ALWAYS_INLINE explicit
		operator Base const *() const { return m_ptr; }

		NO_DISCARD ALWAYS_INLINE
		Base* operator ->() { return m_ptr; }

		NO_DISCARD ALWAYS_INLINE
		Base const* operator ->() const { return m_ptr; }

		NO_DISCARD ALWAYS_INLINE
		Base& operator *() { return *m_ptr; }

		NO_DISCARD ALWAYS_INLINE
		Base const& operator *() const { return *m_ptr; }

	private:
		Base* m_ptr;

		Unique() = default;
	};
} }

template <typename T>
using Unique = core::containers::Unique<T>;
