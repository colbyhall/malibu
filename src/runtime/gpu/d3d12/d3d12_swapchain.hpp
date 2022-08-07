#pragma once

#include "../swapchain.hpp"
#include "../resources.hpp"

#include "d3d12_utility.hpp"

class D3D12Swapchain : public gpu::SwapchainInterface {
	static constexpr usize frame_count = 2;
public:
	explicit D3D12Swapchain(void* window_handle);

	gpu::Texture const& back_buffer() const override;
	void present() override;
	void wait_for_previous() override;

private:
	ComPtr<IDXGISwapChain3> m_swapchain;
	Array<gpu::Texture> m_back_buffers;
	u8 m_current;

	ComPtr<ID3D12Fence> m_fence;
	HANDLE m_fence_event;
	u64 m_fence_value;
};