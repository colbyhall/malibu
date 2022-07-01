#pragma once

namespace core {
	template <typename T>
	struct MethodOuter;

	template <typename R, typename O, typename... A> 
	struct MethodOuter<R(O::*)(A...)> { using Type = O; };

	template <typename R, typename O, typename... A> 
	struct MethodOuter<R(O::*)(A...)&> { using Type = O; };

	template <typename R, typename O, typename... A> 
	struct MethodOuter<R(O::*)(A...)&&> { using Type = O; };

	template <typename R, typename O, typename... A> 
	struct MethodOuter<R(O::*)(A...) const> { using Type = O; };

	template <typename R, typename O, typename... A> 
	struct MethodOuter<R(O::*)(A...) const&> { using Type = O; };

	template <typename R, typename O, typename... A> 
	struct MethodOuter<R(O::*)(A...) const&&> { using Type = O; };

	template <typename R, typename O, typename... A> 
	struct MethodOuter<R(O::*)(A...) volatile> { using Type = O; };

	template <typename R, typename O, typename... A> 
	struct MethodOuter<R(O::*)(A...) volatile&> { using Type = O; };

	template <typename R, typename O, typename... A> 
	struct MethodOuter<R(O::*)(A...) volatile&&> { using Type = O; };

	template <typename R, typename O, typename... A> 
	struct MethodOuter<R(O::*)(A...) const volatile> { using Type = O; };

	template <typename R, typename O, typename... A> 
	struct MethodOuter<R(O::*)(A...) const volatile&> { using Type = O; };

	template <typename R, typename O, typename... A> 
	struct MethodOuter<R(O::*)(A...) const volatile&&> { using Type = O; };

	/**
	 * Type trait which yields the type of the class given a pointer to a member function of that class, e.g.:
	 *
	 * MethodOuterType<decltype(&Vec3f32::dot)>::Type == Vec3f32
	 */
	template <typename T>
	using MethodOuterType = typename MethodOuter<T>::Type;
}