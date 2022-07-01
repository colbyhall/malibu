#pragma once

#include "invoke.hpp"

namespace core {
	namespace hidden {
		template <typename T>
		T&& declval();

		template <typename T>
		struct Void {
			using Type = void;
		};

		template <typename, typename C, typename... A>
		inline constexpr bool is_invocable_impl = false;

		template <typename C, typename... A>
		inline constexpr bool is_invocable_impl<typename Void<decltype(invoke(declval<C>(), declval<A>()...))>::Type, C, A...> = true;
	}

	/**
	 * Traits class which tests if an instance of CallableType can be invoked with
	 * a list of the arguments of the types provided.
	 *
	 * Examples:
	 *     is_invocable<void()>::Value == true
	 *     is_invocable<void(), FString>::Value == false
	 *     is_invocable<void(FString), FString>::Value == true
	 *     is_invocable<void(FString), const TCHAR*>::Value == true
	 *     is_invocable<void(FString), int32>::Value == false
	 *     is_invocable<void(char, float, bool), int, int, int>::Value == true
	 *     is_invocable<TFunction<void(FString)>, FString>::Value == true
	 *     is_invocable<TFunction<void(FString)>, int32>::Value == false
	 */
	template <typename C, typename... A>
	inline constexpr bool is_invocable = hidden::is_invocable_impl<void, C, A...>;
}