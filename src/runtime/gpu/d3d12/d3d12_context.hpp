#pragma once

#include "../context.hpp"
#include "../resources.hpp"
#include "d3d12_utility.hpp"

#include "async/mutex.hpp"
#include "async/atomic.hpp"
#include "library.hpp"

using namespace core::async;

class D3D12FreeSlotDescriptorHeap {
public:
	explicit D3D12FreeSlotDescriptorHeap() = default;
	explicit D3D12FreeSlotDescriptorHeap(
		ComPtr<ID3D12Device1> device,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		usize cap, 
		bool shader_visible
	);

	D3D12_CPU_DESCRIPTOR_HANDLE alloc() const;
	void free(D3D12_CPU_DESCRIPTOR_HANDLE handle) const;

private:
	ComPtr<ID3D12DescriptorHeap> m_heap;
	Array<Atomic<bool>> m_free_slots;
	usize m_size = 0;
	usize m_cap = 0;
};

struct BindlessHandle {
	D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle;
	u32 index;
};

class D3D12BindlessDescriptorHeap {
public:
	static inline constexpr usize TEXTURE2D_COUNT = 1024;
	static inline constexpr usize TEXTURE2D_INDEX = 1;

	explicit D3D12BindlessDescriptorHeap() = default;
	explicit D3D12BindlessDescriptorHeap(ComPtr<ID3D12Device1> device);

	BindlessHandle alloc_texture2d() const;
	void free_texture2d(BindlessHandle handle) const;
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_texture2d_table() const;

	NO_DISCARD inline ComPtr<ID3D12DescriptorHeap> heap() const { return m_heap; }

private:
	ComPtr<ID3D12DescriptorHeap> m_heap;
	Array<Atomic<bool>> m_free_slots;
	usize m_size;
};

typedef HRESULT (__stdcall *PFN_D3D12_SERIALIZE_ROOT_SIGNATURE)(const D3D12_ROOT_SIGNATURE_DESC *root_signature_desc,D3D_ROOT_SIGNATURE_VERSION version,ID3DBlob **blob,ID3DBlob **error_blob);

struct D3D12QueuedWork {
	ComPtr<ID3D12Fence> fence;
	Array<gpu::Texture> textures;
	Array<gpu::Buffer> buffers;
};

class D3D12Context : public gpu::ContextInterface {
	using Library = core::library::Library;

	using CreateDevice = PFN_D3D12_CREATE_DEVICE;
	using SerializeRootSignature = PFN_D3D12_SERIALIZE_ROOT_SIGNATURE;
	using GetDebugInterface = PFN_D3D12_GET_DEBUG_INTERFACE;

public:
	explicit D3D12Context();

	gpu::Backend backend() const override { return gpu::Backend::D3D12; }
	void post_init() override;

	Library d3d12_library;
	CreateDevice create_device;
	SerializeRootSignature serialize_root_signature;
	GetDebugInterface get_debug_interface;

	ComPtr<ID3D12Device1> device;
	ComPtr<IDXGIFactory4> factory;
	ComPtr<ID3D12CommandQueue> queue;
	ComPtr<ID3D12CommandAllocator> command_allocator;
	ComPtr<ID3D12RootSignature> root_signature;
	D3D12FreeSlotDescriptorHeap rtv_heap;
	D3D12FreeSlotDescriptorHeap dsv_heap;
	
	D3D12BindlessDescriptorHeap bindless_heap;
	Option<gpu::Texture> bindless_texture;

	mutable Array<D3D12QueuedWork> work_queue; // TODO: Make thread safe

#if BUILD_DEBUG
	ComPtr<ID3D12Debug> debug_interface;
#endif

};