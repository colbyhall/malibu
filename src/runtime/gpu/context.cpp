#include "context.hpp"

#include "d3d12/d3d12_context.hpp"

namespace gpu {
	const Context& Context::the() {
		static Context context;
		return context;
	}

	Context::Context() 
		: m_interface (Unique<ContextInterface>::make(D3D12Context()))
	{
			
	}
}