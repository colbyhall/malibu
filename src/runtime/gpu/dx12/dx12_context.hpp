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
	
	Array<gpu::Texture> backbuffers;
	u8 current;
	
	ComPtr<ID3D12Fence> fence;
	HANDLE fence_event;
	u64 fence_value;
};

class Dx12DescriptorHeap {
public:
	explicit Dx12DescriptorHeap() = default;
	explicit Dx12DescriptorHeap(
		ComPtr<ID3D12Device1> device,
		D3D12_DESCRIPTOR_HEAP_TYPE type, 
		usize cap, 
		bool shader_visible
	);

	D3D12_CPU_DESCRIPTOR_HANDLE alloc();
	void free(D3D12_CPU_DESCRIPTOR_HANDLE handle);

private:
	ComPtr<ID3D12DescriptorHeap> m_heap;
	Array<bool> m_free_slots;
	usize m_size;
	usize m_cap;
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
	ComPtr<ID3D12RootSignature> root_signature;
	mutable Dx12DescriptorHeap rtv_heap; // TODO: Bindless under lock

	Option<Dx12Swapchain> swapchain;

};