#include "core/core.hpp"
#include "core/platform/window.hpp"

using namespace core::platform;
using namespace core::time;

#include "gpu/context.hpp"

#include <cstdio>

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

	auto last_frame = Instant::now();
	while (g_running) {
		const auto now = Instant::now();
		const auto delta = now.duration_since(last_frame);
		last_frame = now;

		printf("%f\n", delta.as_secs_f64());

		Window::pump_events();
	}
}