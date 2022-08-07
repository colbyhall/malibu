#pragma once

#include "core.hpp"
#include "containers/unique.hpp"

namespace gpu {
	class Texture;

	class SwapchainInterface {
	public:
		virtual Texture const& back_buffer() const = 0;
		virtual void present() = 0;
		virtual void wait_for_previous() = 0;
	};

	class Swapchain {
	public:
		static Swapchain make(void* window_handle);
		
		NO_DISCARD inline Texture const& back_buffer() const { return m_interface->back_buffer(); }
		inline void present() { m_interface->present(); }
		inline void wait_for_previous() { m_interface->wait_for_previous(); }

		template <typename T = SwapchainInterface>
		T const& interface() const {
			static_assert(core::is_base_of<SwapchainInterface, T>, "T is not derived of SwapchainInterface");
			return static_cast<const T&>(*m_interface);
		}

	private:
		Swapchain(Unique<SwapchainInterface>&& interface) : m_interface(core::move(interface)) { }

		Unique<SwapchainInterface> m_interface;
	};
}