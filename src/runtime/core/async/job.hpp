#pragma once

namespace core::async {
	struct Job {
		void (*task)(void* param) = nullptr;
		void* data = nullptr;

		Job() = default;
		inline Job(void (*_task)(void* data)) : task(_task) {}
		inline explicit Job(void (*_task)(void* data), void* _data) : task(_task), data(_data) {}
	};
}