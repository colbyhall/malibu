#pragma once

#include "../containers/option.hpp"
#include "../containers/string.hpp"
#include "../type_traits.hpp"
#include "../math/vec2.hpp"

namespace core { namespace platform {
	using WindowHandle = void*;

	enum class WindowEventType {
		Closed,
		ExitRequested,
	};

	struct WindowEvent {
		WindowEventType type;
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
		static Option<Window> create(const WindowConfig& config);

		NO_COPY(Window);

		ALWAYS_INLINE
		Window(Window && m)
		: m_handle(m.m_handle) {
			m.m_handle = nullptr;
		}

		ALWAYS_INLINE
		Window& operator=(Window && m) {
			Window w = core::move(*this);
			m_handle = m.m_handle;
			m.m_handle = 0;
			return *this;
		}

		static void pump_events();

	private:
		WindowHandle m_handle;

		ALWAYS_INLINE explicit
		Window(WindowHandle handle) : m_handle(handle) {}
	};
} }