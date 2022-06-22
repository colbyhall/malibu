#pragma once

#include "../context.hpp"
#include "../resources.hpp"
#include "d3d12_utility.hpp"

#include "async/mutex.hpp"
#include "library.hpp"

namespace core::window {
	class Window;
}

struct D3D12Swapchain {
	static constexpr usize frame_count = 2;

	ComPtr<IDXGISwapChain3> handle;
	
	Array<gpu::Texture> back_buffers;
	u8 current;
	
	ComPtr<ID3D12Fence> fence;
	HANDLE fence_event;
	u64 fence_value;
};

class D3D12DescriptorHeap {
public:
	explicit D3D12DescriptorHeap() = default;
	explicit D3D12DescriptorHeap(
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
	usize m_size = 0;
	usize m_cap = 0;
};

typedef HRESULT (__stdcall *PFN_D3D12_SERIALIZE_ROOT_SIGNATURE)(const D3D12_ROOT_SIGNATURE_DESC *root_signature_desc,D3D_ROOT_SIGNATURE_VERSION version,ID3DBlob **blob,ID3DBlob **error_blob);

class D3D12Context : public gpu::ContextInterface {
	using Library = core::library::Library;

	using CreateDevice = PFN_D3D12_CREATE_DEVICE;
	using SerializeRootSignature = PFN_D3D12_SERIALIZE_ROOT_SIGNATURE;
	using GetDebugInterface = PFN_D3D12_GET_DEBUG_INTERFACE;

public:

	gpu::Backend backend() const override { return gpu::Backend::D3D12; }
	bool register_window(const core::window::Window& window) const override;
	void present() const override;
	const gpu::Texture& back_buffer() const override;

	void wait_for_previous() const;

	Library d3d12_library;
	CreateDevice create_device;
	SerializeRootSignature serialize_root_signature;
	GetDebugInterface get_debug_interface;

	ComPtr<ID3D12Device1> device;
	ComPtr<IDXGIFactory4> factory;
	ComPtr<ID3D12CommandQueue> queue;
	ComPtr<ID3D12CommandAllocator> command_allocator;
	ComPtr<ID3D12RootSignature> root_signature;
	mutable D3D12DescriptorHeap rtv_heap; // TODO: Bindless under lock

#if BUILD_DEBUG
	ComPtr<ID3D12Debug> debug_interface;
#endif

	mutable Option<D3D12Swapchain> swapchain;

	explicit D3D12Context();
};