#pragma once

#include "../context.hpp"
#include "../resources.hpp"
#include "dx12_utility.hpp"

#include "core/sync/mutex.hpp"

namespace core { namespace window { 
	class Window;
} }

struct Dx12Swapchain {
	static constexpr usize frame_count = 2;

	ComPtr<IDXGISwapChain3> handle;
	
	// gpu::Texture backbuffers[frame_count];
	u8 current;
	
	ComPtr<ID3D12Fence> fence;
	HANDLE fence_event;
	u64 fence_value;
};

class Dx12Context : public gpu::ContextInterface {
public:
	explicit Dx12Context();

	gpu::Backend backend() const override { return gpu::Backend::Dx12; }
	bool register_window(const core::window::Window& window) const override;
	void wait_for_previous() const;

	ComPtr<ID3D12Device1> device;
	ComPtr<IDXGIFactory4> factory;
	ComPtr<ID3D12CommandQueue> queue;
	ComPtr<ID3D12CommandAllocator> command_allocator;

	Option<Dx12Swapchain> swapchain;
};