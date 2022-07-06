#include "job.hpp"

#include "../containers/array.hpp"
#include "thread.hpp"
#include "fiber.hpp"
#include "mpmc.hpp"

#include <type_traits>

namespace core::async {
	using JobQueue = MPMCQueue<Job>;

	constexpr usize NUM_FIBERS = 256; 
	constexpr usize NUM_WAITING = 256;

	constexpr usize HIGH_PRIORITY_SIZE = 256;
	constexpr usize NORMAL_PRIORITY_SIZE = 512;
	constexpr usize LOW_PRIORITY_SIZE = 1024;

	struct JobSystem {
		Array<JoinHandle> threads;
		Array<Fiber> fibers;

		JobQueue high_queue;
		JobQueue normal_queue;
		JobQueue low_queue;

		Atomic<bool> running;

		Option<Job> find_work() const {
			// High priority task override waiting fibers
			if (auto high = high_queue.pop()) return high.unwrap();

			// TODO: Waiting list

			if (auto normal = normal_queue.pop()) return normal.unwrap();
			if (auto low = low_queue.pop()) return low.unwrap();

			return Option<Job> {};
		}

		void work_cycle() const {
			while (running.load()) {
				// Look for work
				Option<Job> job;
				{
					// High priority task override waiting fibers
					if (auto high = high_queue.pop()) job = high.unwrap();

					// TODO: Waiting list

					if (auto normal = normal_queue.pop()) job = normal.unwrap();
					if (auto low = low_queue.pop()) job = low.unwrap();
				}

				if (job) {
					auto work = job.unwrap();
					work();
				} else {
					// TODO: Put thread to sleep for some time. We can't have a total event based system as we want to supporter many different types of waiting fibers other than counters
				}
			}
		}
	};

	static Option<JobSystem> g_job_system;

	void schedule(Job&& job) {
		if (g_job_system) {
			auto& job_system = g_job_system.as_ref().unwrap();
			job_system.normal_queue.push(core::forward<Job>(job));
		}
	}

	void init_job_system() {
		auto count = logical_core_count();

#if PLATFORM_WIN32
		// Core 0 is used for interupts 
		count -= 1;
#endif

		Array<JoinHandle> threads;
		threads.reserve(count);

		for (u32 i = 0; i < count; ++i) {
			threads.push(Thread::spawn(false, []() {
				auto& job_system = g_job_system.as_ref().unwrap();
				job_system.work_cycle();
			}));
		}

		Array<Fiber> fibers;
		fibers.reserve(NUM_FIBERS);

		auto high_queue = JobQueue::make(HIGH_PRIORITY_SIZE);
		auto normal_queue = JobQueue::make(NORMAL_PRIORITY_SIZE);
		auto low_queue = JobQueue::make(LOW_PRIORITY_SIZE);

		g_job_system = JobSystem{
			core::move(threads),
			core::move(fibers),
			core::move(high_queue),
			core::move(normal_queue),
			core::move(low_queue),
			true
		};

		auto& job_system = g_job_system.as_ref().unwrap();
		for (u32 i = 0; i < count; ++i) {
			job_system.threads[i].thread().resume();
		}
	}

	void shutdown_job_system() {
		if (g_job_system) {
			auto& job_system = g_job_system.as_ref().unwrap();
			job_system.running.store(false);	
		}
	}
}