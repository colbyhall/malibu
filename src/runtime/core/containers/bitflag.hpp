#pragma once

#include "../type_traits.hpp"

namespace core::containers {
	template <typename T>
	class BitFlag {
		static_assert(core::is_enum<T>, "BitFlags can only be specialized for enums");
		using Underlying = typename core::to_unsigned<typename core::to_underlying_type<T>>;
		static constexpr Underlying underlying(T t) {
			return static_cast<Underlying>(t);
		}
	public:
		BitFlag() : bits(0) { };
		inline BitFlag(T t) : bits(0) {
			this->set(t);
		}

		BitFlag& set(T t, bool value = true) {
			auto b = underlying(t);
			bits |= 1 << b;
			return *this;
		}

		bool is_set(T t) {
			auto b = underlying(t);
			return (bits & (1 << b)) != 0;
		}

		inline explicit
		operator Underlying() const { return bits; }
		
	private:
		Underlying bits;
	};
}
using core::containers::BitFlag;