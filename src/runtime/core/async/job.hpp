#pragma once

#include "../containers/function.hpp"
#include "../containers/shared_ref.hpp"

namespace core::async {
	using Job = Function<void()>;

	template <typename T>
	T&& declval();

	template <typename T>
	using ReturnType = decltype(declval<T>()());

	template <typename T>
	class Future {
		struct Internal {
			Atomic<bool> ready;
			u8 data[sizeof(T)];
		};
	public:
		NO_DISCARD inline bool is_ready() const {
			return m_result->ready.load();
		}

	private:
		explicit inline Future(AtomicSharedRef<Internal>&& result) 
			: m_result(core::move(result)) {}
		inline static Future make() { 
			return Future(AtomicSharedRef<Internal>::make(Internal{
				false,
				{}
			}));
		}

		inline Future clone() const { return Future(m_result.clone()); }
		void set_ready(T&& t) const {
			u8* ptr = const_cast<u8*>(&m_result->data[0]);
			new (ptr) T(core::forward<T>(t));
			m_result->ready.store(true);
		}

		template <typename F>
		friend Future<ReturnType<F>> schedule2(F&& job);

		AtomicSharedRef<Internal> m_result;
	};

	void schedule(Job&&);

	template <typename F>
	Future<ReturnType<F>> schedule2(F&& job) {
		auto future = Future<ReturnType<F>>::make();
		auto functor = Function<ReturnType<F>()>(core::forward<F>(job));
		schedule([moved = core::move(functor), future = future.clone()]() {
		 	future.set_ready(moved());
		});
		return future;
	}

	namespace job {
		void init();
		void shutdown();
	}
}