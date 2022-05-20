#include "core/core.hpp"
#include "core/platform/window.hpp"

using namespace core::platform;

#include "gpu/context.hpp"

static bool g_running = true;

void window_callback(WindowHandle window, const WindowEvent& event) {
	if (event.type == WindowEventType::ExitRequested || event.type == WindowEventType::Closed) {
		g_running = false;
	}
}

int main(int argc, char** argv) {
	auto window = Window::create({
		.size = {1280, 720},
		.title = "Hello World",
		.callback = window_callback,
		.visibility = WindowVisibility::Visible,
	}).unwrap();

	const auto& context = gpu::Context::the();

	while (g_running) {
		Window::pump_events();
	}
}