#pragma once

#include "containers/option.hpp"
#include "containers/string.hpp"
#include "templates/type_traits.hpp"
#include "math/vec2.hpp"

namespace core::window {
	using WindowHandle = void*;

	enum class WindowEventType {
		Closed,
		ExitRequested,
		MouseMoved,
		MouseButton,
		Key,
	};

	struct MouseMovedEvent {
		Vec2i32 delta;
	};

	enum class MouseButton : u8 {
		Left,
		Middle,
		Right,
		Count,
	};

	struct MouseButtonEvent {
		bool pressed;
		MouseButton button;
	};

	struct KeyEvent {
		bool pressed;
		int vk; // TODO: Make a platform independent key 
	};

	struct WindowEvent {
		WindowEventType type;
		union {
			MouseMovedEvent mouse_moved;
			MouseButtonEvent mouse_button;
			KeyEvent key;
		};
	};

	using WindowCallback = void(*)(WindowHandle window, const WindowEvent& event);

	enum class WindowVisibility {
		Default,
		Hidden,
		Visible,
		Maximized,
		Minimized,
	};

	struct WindowConfig {
		Vec2u32 size = { 1280, 720 };
		StringView title = "Application";
		WindowCallback callback;
		WindowVisibility visibility = WindowVisibility::Default;
	};

	class Window {
	public:
		static Option<Window> make(const WindowConfig& config);
		static void pump_events(); 

		NO_COPY(Window);

		inline Window(Window && m) noexcept
		: m_handle(m.m_handle) {
			m.m_handle = nullptr;
		}

		inline Window& operator=(Window && m) noexcept {
			Window w = core::move(*this);
			m_handle = m.m_handle;
			m.m_handle = 0;
			return *this;
		}

		bool set_visibility(WindowVisibility visibility);

		bool set_cursor_lock(bool locked);
		void set_cursor_visbility(bool visible);

		inline WindowHandle handle() const { return m_handle; }
		NO_DISCARD Vec2u32 client_size() const;

	private:
		WindowHandle m_handle;

		inline explicit Window(WindowHandle handle) : m_handle(handle) {}
	};
}