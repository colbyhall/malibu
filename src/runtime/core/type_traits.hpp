#pragma once

#include <type_traits>

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
	constexpr T&& forward(typename RemoveReference<T>::Type& t) {
		return static_cast<T&&>(t);
	}
	
	template <typename T>
	constexpr T&& forward(typename RemoveReference<T>::Type&& t) {
		return static_cast<T&&>(t);
	}
	
	template <typename T>
	constexpr typename RemoveReference<T>::Type&& move(T&& t) {
		return static_cast<typename RemoveReference<T>::Type&&>(t);
	}
	
	#define NO_COPY(type) type(const type& t) = delete; type& operator=(const type& c) = delete
	
	template <typename T>
	inline constexpr bool is_void = std::is_void_v<T>;
	
	template <typename T>
	inline constexpr bool is_null_pointer = std::is_null_pointer_v<T>;
	
	template <typename T>
	inline constexpr bool is_integer = std::is_integral_v<T>;
	
	template <typename T>
	inline constexpr bool is_float = std::is_floating_point_v<T>;
	
	template <typename T>
	inline constexpr bool is_enum = std::is_enum_v<T>;
	
	template <typename T>
	inline constexpr bool is_union = std::is_union_v<T>;

	template <typename Base, typename Derived>
	inline constexpr bool is_base_of = std::is_base_of_v<Base, Derived>;
	
	template <typename T>
	using to_underlying_type = std::underlying_type_t<T>;
	
	template <typename T>
	using to_unsigned = std::make_unsigned_t<T>;

}