#include "core/minimal.hpp"
#include "core/window.hpp"
#include "core/sync/mutex.hpp"
#include "core/fs.hpp"

using namespace core;
using namespace core::window;
using namespace core::time;
using namespace core::sync;

#include "gpu/minimal.hpp"

#include <cstdio>

static bool g_running = true;

void window_callback(WindowHandle window, const WindowEvent& event) {
	if (event.type == WindowEventType::ExitRequested || event.type == WindowEventType::Closed) {
		g_running = false;
	}
}

int main(int argc, char** argv) {
	auto window = Window::make({
		.size = { 1280, 720 },
		.title = "Hello World",
		.callback = window_callback,
		.visibility = WindowVisibility::Visible,
	}).unwrap();

	const gpu::Context& context = gpu::Context::the();

	const auto registered = context.register_window(window);
	VERIFY(registered);

	String foo = fs::read_to_string("foo.txt").unwrap();
	auto binary = gpu::compile_hlsl(foo, gpu::ShaderType::Vertex).unwrap();

	auto last_frame = Instant::now();
	while (g_running) {
		const auto now = Instant::now();
		const auto delta = now.duration_since(last_frame);
		last_frame = now;

		Window::pump_events();
	}

	return 0;
}