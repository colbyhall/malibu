#include "job.hpp"

#include "../containers/array.hpp"
#include "thread.hpp"
#include "fiber.hpp"
#include "mpmc.hpp"

namespace core::async {
	using JobQueue = MPMCQueue<Job>;

	constexpr usize NUM_FIBERS = 256; 
	constexpr usize NUM_WAITING = 256;

	constexpr usize HIGH_PRIORITY_SIZE = 256;
	constexpr usize NORMAL_PRIORITY_SIZE = 512;
	constexpr usize LOW_PRIORITY_SIZE = 1024;

	struct JobSystem {
		Array<Thread> threads;
		Array<Fiber> fibers;

		JobQueue high_queue;
		JobQueue normal_queue;
		JobQueue low_queue;
	
		static JobSystem init() {
			auto count = logical_core_count();

#if PLATFORM_WIN32
			// Core 0 is used for interupts 
			count -= 1;
#endif

			Array<Thread> threads;
			threads.reserve(count);

			Array<Fiber> fibers;
			fibers.reserve(NUM_FIBERS);

			auto high_queue = JobQueue::make(HIGH_PRIORITY_SIZE);
			auto normal_queue = JobQueue::make(NORMAL_PRIORITY_SIZE);
			auto low_queue = JobQueue::make(LOW_PRIORITY_SIZE);

			return JobSystem{
				core::move(threads),
				core::move(fibers),
				core::move(high_queue),
				core::move(normal_queue),
				core::move(low_queue)
			};
		}

		static const JobSystem& the() {
			static JobSystem it = init();
			return it;
		}
	};

}