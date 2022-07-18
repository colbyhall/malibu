#include "fiber.hpp"
#include "../memory.hpp"
#include "../win32.hpp"

namespace core::async {
	Fiber::~Fiber() {
		if (!m_handle) return;

		if (!m_thread) DeleteFiber(m_handle);
		m_handle = nullptr;
	}

	void Fiber::switch_to() {
		SwitchToFiber(m_handle);
	}

	static void WINAPI FiberProc(_In_ LPVOID lpParameter) {
		auto& info = *reinterpret_cast<Function<void()>*>(lpParameter);
		info();
		info.~Function();
		mem::free(&info);
	}

	Fiber spawn_fiber(Function<void()>&& spawn) {
		Function<void()>* param = mem::alloc<Function<void()>>();
		new(param) Function<void()>(forward<Function<void()>>(spawn));

		auto* handle = CreateFiber(0, &FiberProc, param);
		return Fiber{ false, handle };
	}

	Fiber convert_thread() {
		auto* handle = ConvertThreadToFiber(nullptr);
		return Fiber{ true, handle };
	}
}