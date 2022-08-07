#include "context.hpp"

#include "d3d12/d3d12_context.hpp"

namespace gpu {
	static Option<Context> g_context = NONE;

	void init() {
		g_context = Context(Unique<ContextInterface>::make<D3D12Context>());
	}

	const Context& Context::the() {
		return g_context.as_ref().unwrap();
	}
}