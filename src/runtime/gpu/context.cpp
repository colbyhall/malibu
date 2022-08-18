#include "context.hpp"

#include "d3d12/d3d12_context.hpp"

namespace gpu {
	static Option<Context> g_context = NONE;

	void init() {
		auto interface = Unique<ContextInterface>::make<D3D12Context>();
		g_context = Context(core::move(interface));
		g_context.as_mut().unwrap().post_init();
	}

	const Context& Context::the() {
		return g_context.as_ref().unwrap();
	}
}