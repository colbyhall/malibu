#pragma once

#include "core/containers/unique.hpp"

namespace gpu {

	enum class Backend {
		Dx12,
	};

	class ContextInterface {
	public:
		virtual Backend backend() const = 0;
	};

	class Context {
	public:
		static const Context& the();

	private:
		Context();

		Unique<ContextInterface> m_interface;
	};

}

