#pragma once

#include "core.hpp"
#include "containers/unique.hpp"

namespace gpu {
	enum class Backend {
		D3D12,
	};

	class ContextInterface {
	public:
		virtual Backend backend() const = 0;
		virtual void post_init() = 0;
	};

	class Context {
		
	public:
		static const Context& the();

		NO_DISCARD inline Backend backend() const { return m_interface->backend(); }

		template <typename T = ContextInterface>
		T const& interface() const { 
			static_assert(core::is_base_of<ContextInterface, T>, "T is not derived of ContextInterface");
			return static_cast<const T&>(*m_interface);
		}

	private:
		inline Context(Unique<ContextInterface>&& interface) 
			: m_interface(core::forward< Unique<ContextInterface>>(interface)) {}

		inline void post_init() { return m_interface->post_init(); }

		friend void init();

		Unique<ContextInterface> m_interface;
	};

	void init();
}

