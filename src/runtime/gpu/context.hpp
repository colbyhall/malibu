#pragma once

#include "core.hpp"
#include "window.hpp"
#include "containers/unique.hpp"

namespace core::window {
	class Window;
}

namespace gpu {
	class Texture;

	enum class Backend {
		D3D12,
	};

	using namespace core::window;

	class ContextInterface {
	public:
		virtual Backend backend() const = 0;
		virtual bool register_window(const Window& window) const = 0;
		virtual void present() const = 0;
		virtual const Texture& back_buffer() const = 0;
	};

	class Context {
		
	public:
		static const Context& the();

        NO_DISCARD inline Backend backend() const { return m_interface->backend(); }
        NO_DISCARD inline bool register_window(const Window& window) const { return m_interface->register_window(window); }
		NO_DISCARD inline const Texture& back_buffer() const { return m_interface->back_buffer(); }
		inline void present() const { return m_interface->present(); }

		template <typename T = ContextInterface>
		T const& interface() const { 
			static_assert(core::is_base_of<ContextInterface, T>, "T is not derived of ContextInterface");
			return static_cast<const T&>(*m_interface);
		}

	private:
		Context();

		Unique<ContextInterface> m_interface;
	};

}

