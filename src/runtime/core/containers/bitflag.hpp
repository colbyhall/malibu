#pragma once

#include "type_traits.hpp"

template <typename T>
class BitFlag {
	static_assert(core::is_enum<T>, "BitFlags can only be specialized for enums");
	using Underlying = typename core::to_unsigned<typename core::to_underlying_type<T>>;
	static constexpr Underlying underlying(T t) {
		return static_cast<Underlying>(t);
	}
	Underlying bits;

public:
	BitFlag() : bits(0) { };
	ALWAYS_INLINE
		BitFlag(T t) : bits(0) {
		this->set(t);
	}

	BitFlag& set(T t, bool value = true) {
		auto b = underlying(t);
		bits |= 1 << b;
		return *this;
	}

	NO_DISCARD ALWAYS_INLINE explicit
	operator Underlying() const { return bits; }
};