#pragma once

#include "containers/option.hpp"
#include "containers/string.hpp"
#include "templates/type_traits.hpp"
#include "math/aabb2.hpp"

#include "swapchain.hpp"

#include "element.hpp"

namespace gui {
	using WindowHandle = void*;

	enum class WindowEventType {
		Closed,
		ExitRequested,
		MouseMoved,
		MouseButton,
		MouseWheel,
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

	struct MouseWheelEvent {
		f32 delta;
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
			MouseWheelEvent mouse_wheel;
			KeyEvent key;
		};
	};

	using WindowCallback = void(*)(WindowHandle window, const WindowEvent& event);

	struct WindowConfig {
		Vec2u32 size = { 1280, 720 };
		StringView title = "Application";
		Visibility visibility;
	};

	class Window {
	public:
		static SharedRef<Window> make(const WindowConfig& config);
		static void pump_events();

		NO_COPY(Window);

		inline Window(Window&& m) noexcept : 
			m_handle(m.m_handle), 
			m_swapchain(core::move(m.m_swapchain)), 
			m_element(core::move(m.m_element)) 
		{
			m.m_handle = nullptr;
		}

		inline Window& operator=(Window&& m) noexcept {
			Window w = core::move(*this);
			m_handle = m.m_handle;
			m_swapchain = core::move(m.m_swapchain);
			m_element = core::move(m.m_element);

			m.m_handle = 0;
			return *this;
		}

		NO_DISCARD Rect2u32 client() const;
		bool set_visibility(Visibility visibility);
		bool set_cursor_lock(bool locked);
		void set_cursor_visbility(bool visible);

		inline Option<SharedRef<Element> const&> element_ref() const { return m_element.as_ref(); }
		void set_element(SharedRef<Element>&& element);

		inline WindowHandle handle() const { return m_handle; }
		f32 dpi() const;

		inline gpu::Swapchain const & swapchain_ref() const { return m_swapchain; }
		inline gpu::Swapchain& swapchain_mut() { return m_swapchain; }

		// TODO: Make this private
		void on_paint();
		void on_resize();

	private:

		inline explicit Window(WindowHandle handle, gpu::Swapchain&& swapchain) 
			: m_handle(handle), m_swapchain(core::forward<gpu::Swapchain>(swapchain)) {}

		WindowHandle m_handle;
		gpu::Swapchain m_swapchain;

		Option<SharedRef<Element>> m_element = NONE;
	};
}