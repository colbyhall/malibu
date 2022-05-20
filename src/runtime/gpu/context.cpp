#include "context.hpp"

#include "dx12/dx12_context.hpp"

namespace gpu {
	const Context& Context::the() {
		static Context context;
		return context;
	}

	Context::Context() 
		: m_backend (Backend::Dx12)
		, m_interface (Unique<ContextInterface>(Dx12Context())) 
	{
			
	}
}