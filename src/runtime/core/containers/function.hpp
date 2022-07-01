#pragma once

#include "../memory.hpp"
#include "../templates/remove_cv.hpp"
#include "../templates/enable_if.hpp"
#include "../templates/decay.hpp"
#include "../templates/is_invocable.hpp"
#include "../templates/is_constructible.hpp"
#include "../templates/type_traits.hpp"
#include <new>

namespace core::containers {
	template <typename F>
	class Function;
	
	template <typename T>
	inline constexpr bool is_function = false;
	template <typename T>
	inline constexpr bool is_function<Function<T>> = true;

	template <typename F>
	class FunctionRef;

	template <typename T>
	inline constexpr bool is_function_ref = false;
	template <typename T>
	inline constexpr bool is_function_ref<FunctionRef<T>> = true;

	namespace hidden {
		template <typename Functor, typename FuncType>
		struct FunctionRefCaller;

		template <typename Functor, typename R, typename... P>
		struct FunctionRefCaller<Functor, R(P...)>
		{
			static R call(void* obj, P&... params)
			{
				return invoke(*(Functor*)obj, forward<P>(params)...);
			}
		};

		template <typename Functor, typename... P>
		struct FunctionRefCaller<Functor, void(P...)>
		{
			static void call(void* obj, P&... params)
			{
				invoke(*(Functor*)obj, forward<P>(params)...);
			}
		};

		template <typename S, typename F>
		class FunctionBase;

		template <typename S, typename R, typename... Param>
		class FunctionBase<S, R(Param...)> {
			template <typename OtherS, typename OtherF>
			friend class FunctionBase;
		public:
			FunctionBase() = delete;

			FunctionBase(FunctionBase&& other) 
				: m_callable(other.m_callable), m_storage(move(other.storage)) {
				if (m_callable) {
					other.m_callable = nullptr;
				}
			}

			template <
				typename F,
				typename = EnabledIf<!is_same<FunctionBase, Decayed<F>>>
			>
			FunctionBase(F&& f) {
				if (auto* binding = m_storage.bind(forward<F>(f))) {
					using DecayedFunctorType = PointerRemoved<decltype(binding)>;
					m_callable = &FunctionRefCaller<DecayedFunctorType, R (Param...)>::call;
				}
			}

			FunctionBase& operator=(FunctionBase&&) = delete;
			FunctionBase& operator=(const FunctionBase&) = delete;

			R operator()(Param... params) const {
				VERIFY(m_callable);
				return (m_callable)(m_storage.ptr(), params...);
			}

		private:
			R (*m_callable)(void*, Param&...);
			S m_storage;
		};
		

		struct RefStorage {
			RefStorage() = default;
			template <typename F>
			ReferenceRemoved<F>* bind(F&& f) {
				m_ptr = (void*)&f;
				return &f;
			}

			void* ptr() const { return m_ptr; }
		private:
			void* m_ptr = nullptr;
		};

		struct FunctionWrapperInterface {
			virtual ~FunctionWrapperInterface() = default;
		};

		template <typename T>
		struct FunctionWrapper : public FunctionWrapperInterface {
			template <typename... A>
			explicit FunctionWrapper(A&&... args) : t(forward<A>(args)...) {}

			T t;
		};

		struct UniqueStorage {
			UniqueStorage() = default;
			template <typename F>
			ReferenceRemoved<F>* bind(F&& f) {
				void* memory = mem::alloc(mem::Layout::single<F>);
				auto* result = new(memory) F(forward<F>(f));
				m_ptr = memory;

				return result;
			}

			~UniqueStorage() {
				auto* f = (FunctionWrapperInterface*)m_ptr;
				f->~FunctionWrapperInterface();
				mem::free(m_ptr);
				m_ptr = nullptr;
			}

			void* ptr() const { return m_ptr; }
		private:
			void* m_ptr = nullptr;
		};

		template <typename T>
		T&& declval();

		template <typename F, typename R, typename... P>
		inline constexpr bool functor_return_type_is_compatible = is_constructible<R, decltype(declval<F>()(declval<P>()...))>;
		template <typename MR, typename Class, typename R, typename... P>
		inline constexpr bool functor_return_type_is_compatible<MR Class::*, R, P...> = is_constructible<R, MR>;
		template <typename MR, typename Class, typename R, typename... P>
		inline constexpr bool functor_return_type_is_compatible<MR Class::* const, R, P...> = is_constructible<R, MR>;
		template <typename MR, typename Class, typename... MP, typename R, typename... P>
		inline constexpr bool functor_return_type_is_compatible<MR(Class::*)(MP...), R, P...> = is_constructible<R, MR>;
		template <typename MR, typename Class, typename... MP, typename R, typename... P>
		inline constexpr bool functor_return_type_is_compatible<MR(Class::*)(MP...) const, R, P...> = is_constructible<R, MR>;

		template <typename A, typename B>
		inline constexpr bool func_can_bind_to_functor = false;

		template <typename F, typename R, typename... P>
		inline constexpr bool func_can_bind_to_functor<R(P...), F> = is_invocable<F, P...> && functor_return_type_is_compatible<F, R, P...>;

		template <typename F, typename... P>
		inline constexpr bool func_can_bind_to_functor<void(P...), F> = is_invocable<F, P...>;
	}

	template <typename Func>
	class FunctionRef final : public hidden::FunctionBase<hidden::RefStorage, Func> {
		using Super = hidden::FunctionBase<hidden::RefStorage, Func>;
	
	public:
		template <
			typename Functor,
			typename = EnabledIf<
				!is_function_ref<Decayed<Functor>> &&
				hidden::func_can_bind_to_functor<Func, Decayed<Functor>>
			>
		>
		FunctionRef(Functor&& f) : Super(forward<Functor>(f)) { }
		FunctionRef(const FunctionRef&) = default;

		FunctionRef& operator=(const FunctionRef&) const = delete;

		~FunctionRef() = default;
	};

	template <typename Func>
	class Function final : public hidden::FunctionBase<hidden::UniqueStorage, Func> {
		using Super = hidden::FunctionBase<hidden::UniqueStorage, Func>;

	public:
		template <
			typename Functor,
			typename = EnabledIf<
				!is_function<Decayed<Functor>> &&
				hidden::func_can_bind_to_functor<Func, Decayed<Functor>>
			>
		>
		Function(Functor&& f) : Super(forward<Functor>(f)) { }
		Function(const Function&) = default;

		Function& operator=(const Function&) const = delete;

		~Function() = default;
	};
}

using core::containers::Function;
using core::containers::FunctionRef;
