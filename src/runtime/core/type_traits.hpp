#pragma once

#include "primitives.hpp"

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

	template<typename T, T value>
	struct IntegralConstant {
		static constexpr T VALUE = value;

		constexpr operator T() const noexcept { return VALUE; }

		NO_DISCARD constexpr T operator()() const noexcept { return VALUE; }
	};

	template <bool value>
	using BoolConstant = IntegralConstant<bool, value>;

	using True = BoolConstant<true>;
	using False = BoolConstant<false>;

	namespace priv {
		template <bool first_value, typename First, typename... Rest>
		struct _Disjunction { using Type = First; };

		template <typename False, typename Next, typename... Rest>
		struct _Disjunction<false, False, Next, Rest...> {
			using Type = typename _Disjunction<Next::VALUE, Next, Rest...>::Type;
		};

		template <typename... Traits>
		struct Disjunction : False {};

		template <typename First, typename... Rest>
		struct Disjunction<First, Rest...> : _Disjunction<First::VALUE, First, Rest...>::Type { };
	}

	template <typename... Traits>
	inline constexpr bool disjunction = priv::Disjunction<Traits...>::VALUE;

	template <typename, typename>
	inline constexpr bool is_same = false;

	template <typename T>
	inline constexpr bool is_same<T, T> = true;

	template <typename T, typename... Types>
	inline constexpr bool is_any_of = disjunction<is_same<T, Types>...>;
	
	template <typename T>
	inline constexpr bool is_void = is_same<CVRemoved<T>, void>;
	
	template <typename T>
	inline constexpr bool is_null_pointer = is_same<CVRemoved<T>, NullPtr>;
	
	template <typename T>
	inline constexpr bool is_integer = is_any_of<
		CVRemoved<T>, 
		bool, 
		char, signed char, unsigned char,
		u8, u16, u32, u64, usize,
		i8, i16, i32, i64, isize
	>;
	
	template <typename T>
	inline constexpr bool is_float = is_any_of<CVRemoved<T>, f32, f64>;
	
	template <typename T>
	inline constexpr bool is_enum = __is_enum(T);
	
	template <typename T>
	inline constexpr bool is_union = __is_union(T);

	template <typename Base, typename Derived>
	inline constexpr bool is_base_of = __is_base_of(Base, Derived);

	template <typename T>
	inline constexpr bool is_abstract = __is_abstract(T);
	
	template <typename T>
	using to_underlying_type = __underlying_type(T);

	namespace priv {
		template <usize>
		struct _MakeUnsigned;

		template <>
		struct _MakeUnsigned<1> {
			template <typename>
			using Type = u8;
		};

		template <>
		struct _MakeUnsigned<2> {
			template <typename>
			using Type = u16;
		};

		template <>
		struct _MakeUnsigned<4> {
			template <typename>
			using Type = u32;
		};

		template <>
		struct _MakeUnsigned<8> {
			template <typename>
			using Type = u64;
		};

		template <typename T>
		using __MakeUnsigned = typename _MakeUnsigned<sizeof(T)>::template Type<T>;

		template <typename T>
		struct MakeUnsigned {
			using Type = typename __MakeUnsigned<T>;
		};
	}
	
	template <typename T>
	using to_unsigned = typename priv::MakeUnsigned<CVRemoved<T>>::Type;
}