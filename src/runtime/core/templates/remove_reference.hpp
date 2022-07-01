#pragma once

// https://en.cppreference.com/w/cpp/types/remove_reference
// https://en.cppreference.com/w/cpp/types/remove_pointer

namespace core {
	template<typename T>
	struct RemoveReference {
		using Type = T;
	};

	template <typename T>
	struct RemoveReference<T&> {
		using Type = T;
	};

	template <typename T>
	struct RemoveReference<T&&> {
		using Type = T;
	};

	template <typename T>
	using ReferenceRemoved = typename RemoveReference<T>::Type;

	template<typename T>
	struct RemovePointer {
		using Type = T;
	};

	template <typename T>
	struct RemovePointer<T*> {
		using Type = T;
	};

	template <typename T>
	struct RemovePointer<T**> {
		using Type = T;
	};

	template <typename T>
	using PointerRemoved = typename RemovePointer<T>::Type;
}