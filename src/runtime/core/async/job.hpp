#pragma once

#include "../containers/function.hpp"

namespace core::async {
	using Job = Function<void()>;

	void schedule(Job&& job);

	// This api is a bit inconsistent as singletons don't self destruct
	void shutdown();
}