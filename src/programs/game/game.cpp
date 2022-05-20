#include "core/core.hpp"
#include "gpu/context.hpp"

#include <cstdio>

#define NOMINMAX
#include <windows.h>

static
LRESULT CALLBACK window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	switch (Msg) {
		case WM_DESTROY:
		case WM_CLOSE: {
			ExitProcess(0);
		} break;
	}

	return DefWindowProcA(hWnd, Msg, wParam, lParam);
}

int main(int argc, char** argv) {
	HINSTANCE hInstance = GetModuleHandleA(nullptr);

	DWORD dwStyle = WS_OVERLAPPEDWINDOW;

	WNDCLASSEXA window_class = {};
	window_class.cbSize = sizeof(WNDCLASSEXW);
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = &window_proc;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.hInstance = hInstance;
	window_class.hIcon = ::LoadIcon(hInstance, nullptr);
	window_class.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	window_class.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	window_class.lpszMenuName = nullptr;
	window_class.lpszClassName = "window_class";
	window_class.hIconSm = ::LoadIcon(hInstance, nullptr);

	// FIXME: Return error when class registration failed
	const ATOM atom = RegisterClassExA(&window_class);
	if (atom == 0) {
		return -1;
	}

	const LONG WINDOW_WIDTH = 1280;
	const LONG WINDOW_HEIGHT = 720;

	RECT adjusted_rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&adjusted_rect, dwStyle, 0);

	const int width = adjusted_rect.right - adjusted_rect.left;
	const int height = adjusted_rect.bottom - adjusted_rect.top;

	const int monitor_width = GetSystemMetrics(SM_CXSCREEN);
	const int monitor_height = GetSystemMetrics(SM_CYSCREEN);

	const int x = monitor_width / 2 - width / 2;
	const int y = monitor_height / 2 - height / 2;

	HWND hwnd = CreateWindowExA(
		0,
		window_class.lpszClassName,
		"Malibu",
		dwStyle,
		x, y, width, height,
		nullptr, nullptr,
		hInstance,
		nullptr
	);
	// FIXME: Error
	VERIFY(hwnd != INVALID_HANDLE_VALUE);

	ShowWindow(hwnd, SW_SHOWDEFAULT);

	const auto& context = gpu::Context::the();

	for (;;) {
		MSG msg;
		while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}

    return 0;
}