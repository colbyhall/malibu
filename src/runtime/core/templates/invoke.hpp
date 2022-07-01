#pragma once

#include "../types.hpp"
#include "method_outer.hpp"

namespace core {
	namespace hidden {
		template <typename O, typename T>
		inline auto dereference_if_necessary(T&& Target, const volatile O* TargetPtr) -> decltype((T&&)Target)
		{
			// If the target is the same as or is derived from the outer type, just return it unchanged.
			return (T&&)Target;
		}

		template <typename O, typename T>
		inline auto dereference_if_necessary(T&& Target, ...) -> decltype(*(T&&)Target)
		{
			// If the target is not related to the outer type, assume it's a (possibly smart) pointer and dereference it.
			return *(T&&)Target;
		}
	}

	/**
	 * Invokes a callable with a set of arguments.  Allows the following:
	 *
	 * - Calling a functor object given a set of arguments.
	 * - Calling a function pointer given a set of arguments.
	 * - Calling a member function given a reference to an object and a set of arguments.
	 * - Calling a member function given a pointer (including smart pointers) to an object and a set of arguments.
	 * - Projecting via a data member pointer given a reference to an object.
	 * - Projecting via a data member pointer given a pointer (including smart pointers) to an object.
	 *
	 * See: http://en.cppreference.com/w/cpp/utility/functional/invoke
	 */
	template <typename F, typename... A>
	inline auto invoke(F&& f, A&&... args) -> decltype(forward<F>(f)(forward<A>(args)...)) {
		return forward<F>(f)(forward<A>(args)...);
	}

	template <typename R, typename O, typename T>
	inline auto invoke(R O::* pdm, T&& target) -> decltype(hidden::dereference_if_necessary<O>(forward<T>(target), &target).*pdm) {
		return hidden::dereference_if_necessary<O>(forward<T>(target), &target).*pdm;
	}

	template <typename PtrMemFunType, typename T, typename... A, typename O = MethodOuterType<PtrMemFunType>>
	inline auto invoke(PtrMemFunType ptr_mem_fun, T&& Target, A&&... args) -> decltype((hidden::dereference_if_necessary<O>(forward<T>(Target), &Target).*ptr_mem_fun)(forward<A>(args)...))
	{
		return (hidden::dereference_if_necessary<O>(forward<T>(Target), &Target).*ptr_mem_fun)(forward<A>(args)...);
	}
}