#pragma once

namespace core {
	template <typename T, typename... A>
	inline constexpr bool is_constructible = __is_constructible(T, A...);
}