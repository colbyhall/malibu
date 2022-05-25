#include "command_list.hpp"
#include "context.hpp"

#include "dx12/dx12_command_list.hpp"

namespace gpu {
	GraphicsCommandList GraphicsCommandList::create() {
		auto& context = Context::the();

		Option<Unique<GraphicsCommandListInterface>> interface;
		switch (context.backend()) {
			case Backend::Dx12:
				interface = Unique<GraphicsCommandListInterface>(Dx12GraphicsCommandList());
				break;
		}

		return GraphicsCommandList { interface.unwrap() };
	}
}