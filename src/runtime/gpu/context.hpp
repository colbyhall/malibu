#pragma once

#include "core/minimal.hpp"
#include "core/window.hpp"

namespace gpu {

	enum class Backend {
		Dx12,
	};

	using namespace core::window;

	class ContextInterface {
	public:
		virtual Backend backend() const = 0;
		virtual bool register_window(const Window& window) const = 0;
		virtual void present() const = 0;
	};

	class Context {
		
	public:
		static const Context& the();

		ALWAYS_INLINE
		Backend backend() const { return m_interface->backend(); }
		ALWAYS_INLINE
		bool register_window(const Window& window) const { 
			return m_interface->register_window(window);
		}

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

