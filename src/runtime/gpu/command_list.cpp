#include "command_list.hpp"
#include "context.hpp"

#include "d3d12/d3d12_command_list.hpp"

namespace gpu {
	GraphicsCommandList GraphicsCommandList::make() {
		auto& context = Context::the();

		Option<Unique<GraphicsCommandListInterface>> interface = NONE;
		switch (context.backend()) {
			case Backend::D3D12:
				interface = Unique<GraphicsCommandListInterface>::make<D3D12GraphicsCommandList>();
				break;
		}

		return GraphicsCommandList { interface.unwrap() };
	}
}