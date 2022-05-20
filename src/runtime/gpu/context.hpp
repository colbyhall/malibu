#pragma once

#include "core/containers/unique.hpp"

namespace gpu {

	enum class Backend {
		Dx12,
	};

	class ContextInterface {
	public:

	};

	class Context {
	public:
		static const Context& the();

	private:
		Context();

		Backend m_backend;
		Unique<ContextInterface> m_interface;
	};

}

