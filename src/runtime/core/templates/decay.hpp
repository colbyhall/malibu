#pragma once

#include "../types.hpp"
#include "remove_cv.hpp"
#include "remove_reference.hpp"

namespace core {
	namespace hidden {
		template <typename T>
		struct DecayNonReference {
			using Type = CVRemoved<T>;
		};

		template <typename T>
		struct DecayNonReference<T[]> {
			using Type = T*;
		};

		template <typename T, u32 N>
		struct DecayNonReference<T[N]> {
			using Type = T*;
		};

		template <typename R, typename... P>
		struct DecayNonReference<R(P...)> {
			typedef R (*T)(P...);
		};
	}

	 // http://en.cppreference.com/w/cpp/types/decay
	 template <typename T>
	 struct Decay {
		 using Type = typename hidden::DecayNonReference<ReferenceRemoved<T>>::Type;
	 };

	 template <typename T>
	 using Decayed = typename Decay<T>::Type;
}