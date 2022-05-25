#include "dx12_resources.hpp"
#include "dx12_context.hpp"

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