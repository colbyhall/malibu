#include "swapchain.hpp"
#include "context.hpp"

#include "d3d12/d3d12_swapchain.hpp"

namespace gpu {
	Swapchain Swapchain::make(void* window_handle) {
		auto& context = Context::the();

		Option<Unique<SwapchainInterface>> interface;
		switch (context.backend()) {
		case Backend::D3D12:
			interface = Unique<SwapchainInterface>::make<D3D12Swapchain>(window_handle);
			break;
		}

		return Swapchain { interface.unwrap() };
	}
}