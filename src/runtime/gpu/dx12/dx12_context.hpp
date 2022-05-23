#pragma once

#include "../context.hpp"
#include "dx12_utility.hpp"

#include "core/sync/mutex.hpp"
#include "core/window.hpp"

using namespace core::window;

struct Dx12Swapchain {
	ComPtr<IDXGISwapChain1> swapchain;


};

class Dx12Context : public gpu::ContextInterface {
public:
	explicit Dx12Context();

	gpu::Backend backend() const override { return gpu::Backend::Dx12; }
	bool register_window(const Window& window) const override;

	ComPtr<ID3D12Device1> device;
	ComPtr<ID3D12CommandQueue> queue;
	Option<Dx12Swapchain> swapchain;
};