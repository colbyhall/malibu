#include "d3d12_command_list.hpp"
#include "d3d12_context.hpp"
#include "d3d12_pipelines.hpp"
#include "d3d12_resources.hpp"

D3D12GraphicsCommandList::D3D12GraphicsCommandList() {
	auto& context = gpu::Context::the().interface<D3D12Context>();

	throw_if_failed(context.device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		context.command_allocator.Get(),
		nullptr,
		IID_PPV_ARGS(&m_command_list)
	));
	throw_if_failed(m_command_list->Close());
}

void D3D12GraphicsCommandList::begin_recording() {
	auto& context = gpu::Context::the().interface<D3D12Context>();

	throw_if_failed(context.command_allocator->Reset());
	throw_if_failed(m_command_list->Reset(context.command_allocator.Get(), nullptr));
	m_command_list->SetGraphicsRootSignature(context.root_signature.Get());
}

void D3D12GraphicsCommandList::copy_buffer_to_texture(const gpu::Texture& dst, const gpu::Buffer& src) {
	TODO("");
}

void D3D12GraphicsCommandList::copy_buffer_to_buffer(const gpu::Buffer& dst, const gpu::Buffer& src) {
	TODO("");
}

void D3D12GraphicsCommandList::texture_barrier(const gpu::Texture& texture, gpu::Layout old_layout, gpu::Layout new_layout) {
	const auto& interface = texture.interface<D3D12Texture>();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.pResource = interface.m_resource.Get();
	barrier.Transition.StateBefore = layout_to_resource_states(old_layout);
	barrier.Transition.StateAfter = layout_to_resource_states(new_layout);
	m_command_list->ResourceBarrier(1, &barrier);
}

void D3D12GraphicsCommandList::begin_render_pass(const gpu::Texture& attachment) {
	{
		const auto& interface = attachment.interface<D3D12Texture>();
		m_command_list->OMSetRenderTargets(1, &interface.m_rtv_handle, 0, nullptr);
	}

	D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (f32)attachment.size().width;
	viewport.Height = (f32)attachment.size().height;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	m_command_list->RSSetViewports(1, &viewport);

	D3D12_RECT rect = {};
	rect.left = 0;
	rect.top = 0;
	rect.bottom = (LONG)attachment.size().height;
	rect.right = (LONG)attachment.size().width;
	m_command_list->RSSetScissorRects(1, &rect);

	const f32 clear_color[] = { 0.05f, 0.05f, 0.05f, 1.0f };
	const auto& interface = attachment.interface<D3D12Texture>();
	m_command_list->ClearRenderTargetView(interface.m_rtv_handle, clear_color, 0, nullptr);
}

void D3D12GraphicsCommandList::set_scissor(Option<Rect2f32> scissor) {
	TODO("");
}

void D3D12GraphicsCommandList::clear_color(LinearColor color) {
	TODO("");
}

void D3D12GraphicsCommandList::clear_depth(f32 depth) {
	TODO("");
}

void D3D12GraphicsCommandList::set_pipeline(const gpu::GraphicsPipeline& pipeline) {
	auto& interface = pipeline.interface<D3D12GraphicsPipeline>();
	m_command_list->SetPipelineState(interface.m_pipeline.Get());
}

void D3D12GraphicsCommandList::set_vertices(const gpu::Buffer& buffer) {
	auto& interface = buffer.interface<D3D12Buffer>();
	m_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D12_VERTEX_BUFFER_VIEW view = {};
	view.BufferLocation = (UINT)interface.m_resource->GetGPUVirtualAddress();
	view.StrideInBytes = (UINT)interface.m_stride;
	view.SizeInBytes = (UINT)(interface.m_len * interface.m_stride);
	m_command_list->IASetVertexBuffers(0, 1, &view);
}

void D3D12GraphicsCommandList::set_indices(const gpu::Buffer& buffer) {
	TODO("");
}

void D3D12GraphicsCommandList::push_constant(const void* ptr) {
	m_command_list->SetGraphicsRoot32BitConstants(0, 16, ptr, 0);
}

void D3D12GraphicsCommandList::draw(usize vertex_count, usize first_vertex) {
	m_command_list->DrawInstanced((UINT)vertex_count, 1, (UINT)first_vertex, 0);
}

void D3D12GraphicsCommandList::draw_indexed(usize index_count, usize first_index) {
	TODO("");
}

void D3D12GraphicsCommandList::end_render_pass() {
	// Do nothing
}

void D3D12GraphicsCommandList::end_recording() {
	throw_if_failed(m_command_list->Close());
}

void D3D12GraphicsCommandList::submit() {
	auto& context = gpu::Context::the().interface<D3D12Context>();
	ID3D12CommandList* ppCommandLists[] = { m_command_list.Get() };
	context.queue->ExecuteCommandLists(1, ppCommandLists);
}