#include "window.hpp"
#include "win32.hpp"
#include "library.hpp"
using core::library::Library;

typedef enum PROCESS_DPI_AWARENESS {
	PROCESS_DPI_UNAWARE,
	PROCESS_SYSTEM_DPI_AWARE,
	PROCESS_PER_MONITOR_DPI_AWARE
} PROCESS_DPI_AWARENESS;

enum MONITOR_DPI_TYPE {
	MDT_EFFECTIVE_DPI,
	MDT_ANGULAR_DPI,
	MDT_RAW_DPI,
	MDT_DEFAULT
};

typedef HRESULT(*SetProcessDPIAwareness)(PROCESS_DPI_AWARENESS value);
typedef HRESULT(*GetDPIForMonitor)(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);

namespace core::window {
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

	Option<Window> Window::make(const WindowConfig& config) {
		static bool first = true;
		static auto shcore = Library::open("shcore.dll");
		if (first && shcore) {
			first = false;
			auto& actual = shcore.as_ref().unwrap();
			auto SetProcessDpiAwareness = (SetProcessDPIAwareness)actual.find("SetProcessDpiAwareness");
			if (SetProcessDpiAwareness) SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
		}

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
			config.title.ptr(),
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

	Vec2u32 Window::client_size() const {
		RECT rect;
		GetClientRect((HWND)m_handle, &rect);
		const u32 width = rect.right - rect.left;
		const u32 height = rect.bottom - rect.top;
		return { width, height };
	}

	bool Window::set_visibility(WindowVisibility visibility) {
		int show = SW_HIDE;
		switch (visibility) {
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

		return ShowWindow((HWND)m_handle, show) > 0;
	}
}