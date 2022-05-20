#include "../window.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define WIN32_MEAN_AND_LEAN
#include <windows.h>

namespace core { namespace platform {
	static
		LRESULT CALLBACK window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		auto callback = (WindowCallback)GetWindowLongPtrA(hWnd, GWLP_USERDATA);
		const WindowHandle window = hWnd;

		switch (Msg) {
			case WM_DESTROY: {
				callback(window, WindowEvent { WindowEventType::ExitRequested });
			} break;
			case WM_CLOSE: {
				callback(window, WindowEvent { WindowEventType::Closed });
			} break;
		}

		return DefWindowProcA(hWnd, Msg, wParam, lParam);
	}

	Option<Window> Window::create(const WindowConfig& config) {
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
			return Option<Window>();
		}

		RECT adjusted_rect = { 0, 0, (LONG)config.size.width, (LONG)config.size.height };
		AdjustWindowRect(&adjusted_rect, dwStyle, 0);

		const int width = adjusted_rect.right - adjusted_rect.left;
		const int height = adjusted_rect.bottom - adjusted_rect.top;

		const int monitor_width = GetSystemMetrics(SM_CXSCREEN);
		const int monitor_height = GetSystemMetrics(SM_CYSCREEN);

		const int x = monitor_width / 2 - width / 2;
		const int y = monitor_height / 2 - height / 2;

		HWND handle = CreateWindowExA(
			0,
			window_class.lpszClassName,
			&config.title.slice()[0],
			dwStyle,
			x, y, width, height,
			nullptr, nullptr,
			hInstance,
			nullptr
		);
		// FIXME: Error
		VERIFY(handle != INVALID_HANDLE_VALUE);

		int show = 0;
		switch (config.visibility) {
			case WindowVisibility::Default:
				show = SW_SHOWDEFAULT;
				break;
			case WindowVisibility::Visible:
				show = SW_SHOWNORMAL;
				break;
			case WindowVisibility::Maximized:
				show = SW_SHOWMAXIMIZED;
				break;
			case WindowVisibility::Minimized:
				show = SW_SHOWMINIMIZED;
				break;
		}

		if (show != 0) {
			ShowWindow(handle, show);
		}

		SetWindowLongPtrA(handle, GWLP_USERDATA, (LONG_PTR)config.callback);

		return Window(handle);
	}

	void Window::pump_events() {
		MSG msg;
		while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}
} }