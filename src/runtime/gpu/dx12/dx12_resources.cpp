#include "dx12_resources.hpp"
#include "dx12_context.hpp"

Dx12Texture::Dx12Texture(
	BitFlag<gpu::TextureUsage> usage,
	gpu::Format format,
	Vec3u32 size,
	ComPtr<ID3D12Resource> resource
) : m_usage(usage), m_format(format), m_size(size) {
	VERIFY(size.width > 0);
	VERIFY(size.height > 0);
	VERIFY(size.depth > 0);

	auto& context = gpu::Context::the().interface<Dx12Context>();

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
	VERIFY(dxgi_format != DXGI_FORMAT_UNKNOWN);

	if (resource != nullptr) {
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = dimension;
		desc.Width = size.width;
		desc.Height = size.height;
		desc.DepthOrArraySize = size.depth;
		desc.MipLevels = 1;
		desc.Format = dxgi_format;
		desc.SampleDesc.Count = 1;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		const D3D12_RESOURCE_STATES initial_state = D3D12_RESOURCE_STATE_GENERIC_READ;

		D3D12_HEAP_PROPERTIES heap = {};
		heap.Type = D3D12_HEAP_TYPE_DEFAULT;

		throw_if_failed(context.device->CreateCommittedResource(
			&heap,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			initial_state,
			nullptr,
			IID_PPV_ARGS(&m_resource)
		));		
	} else {
		m_resource = resource;
	}

	if (usage.is_set(gpu::TextureUsage::Color_Attachment)) {
		m_rtv_handle = context.rtv_heap.alloc();
	}
}

Dx12Buffer::Dx12Buffer(
	BitFlag<gpu::BufferUsage> usage, 
	gpu::BufferKind kind, 
	usize len, 
	usize stride
) : m_usage(usage), m_kind(kind), m_len(len), m_stride(stride) {
	auto& context = gpu::Context::the().interface<Dx12Context>();

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