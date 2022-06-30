#pragma once

#include "../templates/type_traits.hpp"
#include <new>

namespace core::containers {
	template <typename T, typename E>
	class Result {
	public:
		Result(T&& t) : m_set(true), m_ok(true) {
			auto* p = &m_data[0];
			new (p) T(core::forward<T>(t));
		}

		Result(E&& e) : m_set(true), m_ok(false) {
			auto* p = &m_data[0];
			new (p) E(core::forward<E>(e));
		}

		NO_DISCARD inline bool is_ok() const { return m_ok; }
		explicit inline operator bool() const { return is_ok(); }

		NO_DISCARD inline T unwrap() {
			VERIFY(is_ok());
			m_set = false;

			auto* p = reinterpret_cast<T*>(&m_data[0]);
			return core::move(*p);
		}

		NO_DISCARD inline E unwrap_err() {
			VERIFY(!is_ok());
			m_set = false;
			auto* p = reinterpret_cast<E*>(&m_data[0]);
			return core::move(*p);
		}

		~Result() { /* TODO */ }
	private:
		u8 m_set : 1;
		u8 m_ok : 1; // Results technically can also be null after move
		
		union Internal { 
			T t; 
			E e;
			~Internal() {} // This prevents warning C4624
		};
		alignas(Internal) u8 m_data[sizeof(Internal)];
	};
}
using core::containers::Result;