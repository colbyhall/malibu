#include "dx12_command_list.hpp"
#include "dx12_context.hpp"
#include "dx12_pipelines.hpp"
#include "dx12_resources.hpp"

Dx12GraphicsCommandList::Dx12GraphicsCommandList() {
	auto& context = gpu::Context::the().interface<Dx12Context>();

	throw_if_failed(context.device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		context.command_allocator.Get(),
		nullptr,
		IID_PPV_ARGS(&m_command_list)
	));
	throw_if_failed(m_command_list->Close());
}

void Dx12GraphicsCommandList::begin_recording() {
	auto& context = gpu::Context::the().interface<Dx12Context>();

	throw_if_failed(m_command_list->Reset(context.command_allocator.Get(), nullptr));
}

void Dx12GraphicsCommandList::copy_buffer_to_texture(const gpu::Texture& dst, const gpu::Buffer& src) {
	TODO("");
}

void Dx12GraphicsCommandList::copy_buffer_to_buffer(const gpu::Buffer& dst, const gpu::Buffer& src) {
	TODO("");
}

void Dx12GraphicsCommandList::texture_barrier(const gpu::Texture& texture, gpu::Layout old_layout, gpu::Layout new_layout) {
	const Dx12Texture& interface = texture.interface<Dx12Texture>();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = interface.m_resource.Get();
	barrier.Transition.StateBefore = layout_to_resource_states(old_layout);
	barrier.Transition.StateAfter = layout_to_resource_states(new_layout);
	m_command_list->ResourceBarrier(1, &barrier);
}

void Dx12GraphicsCommandList::begin_render_pass(Slice<gpu::Texture const&> attachments) {
	for (int i = 0; i < attachments.len(); ++i) {
		const Dx12Texture& interface = attachments[i].interface<Dx12Texture>();
		m_command_list->OMSetRenderTargets(1, &interface.m_rtv_handle, 0, nullptr);
	}

	D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (f32)attachments[0].size().width;
	viewport.Height = (f32)attachments[0].size().height;
	m_command_list->RSSetViewports(1, &viewport);

	for (int i = 0; i < attachments.len(); ++i) {
		const f32 clear_color[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		const Dx12Texture& interface = attachments[i].interface<Dx12Texture>();
		m_command_list->ClearRenderTargetView(interface.m_rtv_handle, clear_color, 0, nullptr);
	}
}

void Dx12GraphicsCommandList::set_scissor(Option<Rect2f32> scissor) {
	TODO("");
}

void Dx12GraphicsCommandList::clear_color(LinearColor color) {
	TODO("");
}

void Dx12GraphicsCommandList::clear_depth(f32 depth) {
	TODO("");
}

void Dx12GraphicsCommandList::bind_pipeline(const gpu::GraphicsPipeline& pipeline) {
	auto& interface = pipeline.interface<Dx12GraphicsPipeline>();
	m_command_list->SetPipelineState(interface.m_pipeline.Get());
}

void Dx12GraphicsCommandList::bind_vertices(const gpu::Buffer& buffer) {
	auto& interface = buffer.interface<Dx12Buffer>();
	m_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D12_VERTEX_BUFFER_VIEW view = {};
	view.BufferLocation = (UINT)interface.m_resource->GetGPUVirtualAddress();
	view.StrideInBytes = (UINT)interface.m_stride;
	view.SizeInBytes = (UINT)(interface.m_len * interface.m_stride);
	m_command_list->IASetVertexBuffers(0, 1, &view);
}

void Dx12GraphicsCommandList::bind_indices(const gpu::Buffer& buffer) {
	TODO("");
}

void Dx12GraphicsCommandList::draw(usize vertex_count, usize first_vertex) {
	m_command_list->DrawInstanced((UINT)vertex_count, 0, (UINT)first_vertex, 0);
}

void Dx12GraphicsCommandList::draw_indexed(usize index_count, usize first_index) {
	TODO("");
}

void Dx12GraphicsCommandList::end_render_pass() {
	// Do nothing
}

void Dx12GraphicsCommandList::end_recording() {
	throw_if_failed(m_command_list->Close());
}

void Dx12GraphicsCommandList::submit() {
	auto& context = gpu::Context::the().interface<Dx12Context>();
	ID3D12CommandList* ppCommandLists[] = { m_command_list.Get() };
	context.queue->ExecuteCommandLists(1, ppCommandLists);
}