#pragma once

// https://en.cppreference.com/w/cpp/types/remove_cv

namespace core {
	template <typename T>
	struct RemoveCV {
		using Type = T;
	};

	template <typename T>
	struct RemoveCV<const T> {
		using Type = T;
	};

	template <typename T>
	struct RemoveCV<volatile T> {
		using Type = T;
	};

	template <typename T>
	struct RemoveCV<const volatile T> {
		using Type = T;
	};

	template <typename T>
	using CVRemoved = typename RemoveCV<T>::Type;
}