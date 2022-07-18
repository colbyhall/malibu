#pragma once

#include "../containers/function.hpp"

namespace core::async {
	class JoinHandle;

	class Thread {
	public:
		using Id = u32;
		using Handle = void*;
		using Function = core::containers::Function<void()>;

		static JoinHandle spawn(bool start, Thread::Function&& function);
		static Thread current();

		NO_COPY(Thread);

		inline Thread(Thread&& move) noexcept
			: m_handle(move.m_handle), m_id(move.m_id) {
			move.m_handle = nullptr;
			move.m_id = 0;
		}
		inline Thread& operator=(Thread&& move) noexcept {
			m_handle = move.m_handle;
			m_id = move.m_id;
			return *this;
		}

		void resume() const;

		NO_DISCARD inline Handle handle() const { return m_handle; }
		NO_DISCARD inline Id id() const { return m_id; }

	private:
		inline explicit Thread(void* handle, Id id) : m_handle(handle), m_id(id) {}
		friend JoinHandle;

		Handle m_handle;
		Id m_id;
	};

	class JoinHandle {
	public:
		bool join();
		NO_DISCARD inline const Thread& thread() const { return m_thread; }

		JoinHandle(JoinHandle&&) = default;
		JoinHandle& operator=(JoinHandle&&) = default;

		~JoinHandle();
	private:
		inline explicit JoinHandle(void* handle, Thread::Id id) : m_thread(handle, id), m_joined(false) {}

		friend class Thread;

		Thread m_thread;
		bool m_joined;
	};

	NO_DISCARD u32 logical_core_count();
}