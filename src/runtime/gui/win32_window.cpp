#include "window.hpp"

#define UNICODE
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

// NOTE: Could #include <hidusage.h> for these defines
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

#if 0
auto callback = (WindowCallback)GetWindowLongPtrA(hWnd, GWLP_USERDATA);
const WindowHandle window = hWnd;

switch (Msg) {
case WM_DESTROY: {
	callback(window, WindowEvent{ WindowEventType::ExitRequested });
} break;
case WM_CLOSE: {
	callback(window, WindowEvent{ WindowEventType::Closed });
} break;
case WM_INPUT: {
	UINT dwSize = sizeof(RAWINPUT);
	static BYTE lpb[sizeof(RAWINPUT)];

	GetRawInputData(
		(HRAWINPUT)lParam,
		RID_INPUT,
		lpb,
		&dwSize,
		sizeof(RAWINPUTHEADER)
	);

	RAWINPUT* raw = (RAWINPUT*)lpb;
	if (raw->header.dwType == RIM_TYPEMOUSE && (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == 0) {
		const auto x = raw->data.mouse.lLastX;
		const auto y = raw->data.mouse.lLastY;

		WindowEvent event = {};
		event.type = WindowEventType::MouseMoved;
		event.mouse_moved.delta = { x, y };
		callback(window, event);

		if ((raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) == RI_MOUSE_WHEEL) {
			WindowEvent event = {};
			event.type = WindowEventType::MouseWheel;
			event.mouse_wheel.delta = (f32)(short)(raw->data.mouse.usButtonData) / (f32)WHEEL_DELTA;
			callback(window, event);
		}
	}
} break;
case WM_KEYDOWN: {
	WindowEvent event = {};
	event.type = WindowEventType::Key;
	event.key.vk = (int)wParam;
	event.key.pressed = true;
	callback(window, event);
} break;
case WM_KEYUP: {
	WindowEvent event = {};
	event.type = WindowEventType::Key;
	event.key.vk = (int)wParam;
	callback(window, event);
} break;
case WM_LBUTTONDOWN: {
	SetCapture(hWnd);
	WindowEvent event = {};
	event.type = WindowEventType::MouseButton;
	event.mouse_button.button = MouseButton::Left;
	event.mouse_button.pressed = true;
	callback(window, event);
} break;
case WM_MBUTTONDOWN: {
	SetCapture(hWnd);
	WindowEvent event = {};
	event.type = WindowEventType::MouseButton;
	event.mouse_button.button = MouseButton::Middle;
	event.mouse_button.pressed = true;
	callback(window, event);
} break;
case WM_RBUTTONDOWN: {
	SetCapture(hWnd);
	WindowEvent event = {};
	event.type = WindowEventType::MouseButton;
	event.mouse_button.button = MouseButton::Right;
	event.mouse_button.pressed = true;
	callback(window, event);
} break;
case WM_LBUTTONUP: {
	ReleaseCapture();
	WindowEvent event = {};
	event.type = WindowEventType::MouseButton;
	event.mouse_button.button = MouseButton::Left;
	callback(window, event);
} break;
case WM_MBUTTONUP: {
	ReleaseCapture();
	WindowEvent event = {};
	event.type = WindowEventType::MouseButton;
	event.mouse_button.button = MouseButton::Middle;
	callback(window, event);
} break;
case WM_RBUTTONUP: {
	ReleaseCapture();
	WindowEvent event = {};
	event.type = WindowEventType::MouseButton;
	event.mouse_button.button = MouseButton::Right;
	callback(window, event);
} break;
}
#endif

namespace gui {
	static LRESULT CALLBACK window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		auto* window = (Window*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

		if (window == nullptr) {
			return DefWindowProcW(hWnd, Msg, wParam, lParam);
		}

		switch (Msg) {
		case WM_DESTROY:
		case WM_CLOSE:
			ExitProcess(0);
			break;
		case WM_PAINT:
			window->paint();
			break;
		}

		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}

	SharedRef<Window> Window::make(const WindowConfig& config) {
		// TODO: This should be explicit
		static bool first = true;
		static auto shcore = Library::open("shcore.dll");
		if (first) {
			first = false;

			if (shcore) {
				auto& actual = shcore.as_mut().unwrap();
				auto SetProcessDpiAwareness = (SetProcessDPIAwareness)actual.find("SetProcessDpiAwareness");
				if (SetProcessDpiAwareness) SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
			}
		}

		HINSTANCE hInstance = GetModuleHandleA(nullptr);

		DWORD dwStyle = WS_OVERLAPPEDWINDOW;

		WNDCLASSEXW window_class = {};
		window_class.cbSize = sizeof(WNDCLASSEXW);
		window_class.style = CS_HREDRAW | CS_VREDRAW;
		window_class.lpfnWndProc = &window_proc;
		window_class.cbClsExtra = 0;
		window_class.cbWndExtra = 0;
		window_class.hInstance = hInstance;
		window_class.hIcon = ::LoadIconW(hInstance, nullptr);
		window_class.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
		window_class.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
		window_class.lpszMenuName = nullptr;
		window_class.lpszClassName = L"window_class";
		window_class.hIconSm = ::LoadIconW(hInstance, nullptr);

		// FIXME: Return error when class registration failed
		const ATOM atom = RegisterClassExW(&window_class);
		VERIFY(atom != 0);

		RECT adjusted_rect = { 0, 0, (LONG)config.size.width, (LONG)config.size.height };
		AdjustWindowRect(&adjusted_rect, dwStyle, 0);

		const int width = adjusted_rect.right - adjusted_rect.left;
		const int height = adjusted_rect.bottom - adjusted_rect.top;

		const int monitor_width = GetSystemMetrics(SM_CXSCREEN);
		const int monitor_height = GetSystemMetrics(SM_CYSCREEN);

		const int x = monitor_width / 2 - width / 2;
		const int y = monitor_height / 2 - height / 2;

		WString title;
		title.push(config.title);

		HWND handle = CreateWindowExW(
			0,
			window_class.lpszClassName,
			title.ptr(),
			dwStyle,
			x, y, width, height,
			nullptr, nullptr,
			hInstance,
			nullptr
		);
		// FIXME: Error
		VERIFY(handle != INVALID_HANDLE_VALUE);

		RAWINPUTDEVICE rid;
		rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
		rid.usUsage = HID_USAGE_GENERIC_MOUSE;
		rid.dwFlags = RIDEV_INPUTSINK;
		rid.hwndTarget = handle;
		const auto rid_success = RegisterRawInputDevices(&rid, 1, sizeof(rid));
		VERIFY(rid_success); // FIXME: Do error handling

		if (config.visibility == Visibility::Visible) ShowWindow(handle, SW_SHOWNORMAL);

		auto result = SharedRef<Window>::make(Window(handle, gpu::Swapchain::make(handle)));
		SetWindowLongPtrW(handle, GWLP_USERDATA, (LONG_PTR)result.ptr());
		return result;
	}

	void Window::pump_events() {
		MSG msg;
		while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}

	Rect2u32 Window::client() const {
		RECT rect;
		GetClientRect((HWND)m_handle, &rect);
		return Rect2u32::from_min_max({ (u32)rect.left, (u32)rect.top }, { (u32)rect.right, (u32)rect.bottom });
	}

	bool Window::set_visibility(Visibility visibility) {
		int show = SW_HIDE;
		if (visibility == Visibility::Visible) show = SW_SHOWNORMAL;

		return ShowWindow((HWND)m_handle, show) > 0;
	}

	bool Window::set_cursor_lock(bool locked) {
		if (locked) {
			POINT cursor_pos;
			GetCursorPos(&cursor_pos);
			RECT cursor_rect;
			cursor_rect.left = cursor_pos.x;
			cursor_rect.right = cursor_pos.x;
			cursor_rect.bottom = cursor_pos.y;
			cursor_rect.top = cursor_pos.y;
			return ClipCursor(&cursor_rect) > 0;
		}
		return ClipCursor(nullptr) > 0;
	}

	void Window::set_cursor_visbility(bool visible) {
		ShowCursor(visible);
	}
}