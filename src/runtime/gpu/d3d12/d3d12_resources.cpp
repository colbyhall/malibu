#include "d3d12_resources.hpp"
#include "d3d12_context.hpp"

D3D12Texture::D3D12Texture(
	BitFlag<gpu::TextureUsage> usage,
	gpu::Format format,
	Vec3u32 size,
	ComPtr<ID3D12Resource> resource
) : m_usage(usage), m_format(format), m_size(size) {
	VERIFY(size.width > 0);
	VERIFY(size.height > 0);
	VERIFY(size.depth > 0);

	auto& context = gpu::Context::the().interface<D3D12Context>();

	D3D12_RESOURCE_DIMENSION dimension = D3D12_RESOURCE_DIMENSION_UNKNOWN;
	if (size.width > 1) {
		dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		if (size.height > 1) {
			dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			if (size.depth > 1) {
				dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			}
		}
	}
	VERIFY(dimension != D3D12_RESOURCE_DIMENSION_UNKNOWN);


	const DXGI_FORMAT dxgi_format = format_to_dxgi(format);

	if (resource == nullptr) {
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = dimension;
		desc.Width = size.width;
		desc.Height = size.height;
		desc.DepthOrArraySize = size.depth;
		desc.MipLevels = 1;
		desc.Format = dxgi_format;
		desc.SampleDesc.Count = 1;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (usage.is_set(gpu::TextureUsage::ColorAttachment)) {
			desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}
		if (usage.is_set(gpu::TextureUsage::DepthAttachment)) {
			desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}

		D3D12_RESOURCE_STATES initial_state = D3D12_RESOURCE_STATE_GENERIC_READ;
		if (usage.is_set(gpu::TextureUsage::DepthAttachment)) {
			initial_state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}

		D3D12_HEAP_PROPERTIES heap = {};
		heap.Type = D3D12_HEAP_TYPE_DEFAULT;

		bool optimized_clear = usage.is_set(gpu::TextureUsage::ColorAttachment);
		optimized_clear |= usage.is_set(gpu::TextureUsage::DepthAttachment);

		D3D12_CLEAR_VALUE clear = {};
		D3D12_CLEAR_VALUE* pclear = nullptr;
		if (optimized_clear) {
			clear.Color[3] = 1.f;
			clear.Format = dxgi_format;
			clear.DepthStencil.Depth = 1.f;
			pclear = &clear;
		}

		throw_if_failed(context.device->CreateCommittedResource(
			&heap,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			initial_state,
			pclear,
			IID_PPV_ARGS(&m_resource)
		));		
	} else {
		m_resource = resource;
	}

	if (usage.is_set(gpu::TextureUsage::ColorAttachment)) {
		m_rtv_handle = context.rtv_heap.alloc();
		context.device->CreateRenderTargetView(m_resource.Get(), nullptr, m_rtv_handle);
	}
	if (usage.is_set(gpu::TextureUsage::DepthAttachment)) {
		m_dsv_handle = context.dsv_heap.alloc();
		context.device->CreateDepthStencilView(m_resource.Get(), nullptr, m_dsv_handle);
	}
}

D3D12Buffer::D3D12Buffer(
	BitFlag<gpu::BufferUsage> usage, 
	gpu::BufferKind kind, 
	usize len, 
	usize stride
) : m_usage(usage), m_kind(kind), m_len(len), m_stride(stride) {
	auto& context = gpu::Context::the().interface<D3D12Context>();

	D3D12_HEAP_PROPERTIES heap = {};
	switch (kind) {
	case gpu::BufferKind::Storage:
		heap.Type = D3D12_HEAP_TYPE_DEFAULT;
		break;
	case gpu::BufferKind::Upload:
		heap.Type = D3D12_HEAP_TYPE_UPLOAD;
		break;
	case gpu::BufferKind::Download:
		heap.Type = D3D12_HEAP_TYPE_READBACK;
		break;
	}
	
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = len * stride;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	const D3D12_RESOURCE_STATES initial_state = D3D12_RESOURCE_STATE_GENERIC_READ;

	throw_if_failed(context.device->CreateCommittedResource(
		&heap,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		initial_state,
		nullptr,
		IID_PPV_ARGS(&m_resource)
	));
}

Slice<u8> D3D12Buffer::write() const {
	D3D12_RANGE range = {};
	void* ptr;
	throw_if_failed(m_resource->Map(0, &range, &ptr));
	return Slice { reinterpret_cast<u8*>(ptr), m_len * m_stride };
}

void D3D12Buffer::unmap() const {
	m_resource->Unmap(0, nullptr);
}