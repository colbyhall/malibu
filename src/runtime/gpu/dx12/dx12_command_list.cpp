#include "dx12_command_list.hpp"
#include "dx12_context.hpp"
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

void Dx12GraphicsCommandList::copy_buffer_to_texture(const Texture& dst, const Buffer& src) {
	TODO("");
}

void Dx12GraphicsCommandList::copy_buffer_to_buffer(const Buffer& dst, const Buffer& src) {
	TODO("");
}

void Dx12GraphicsCommandList::texture_barrier(const Texture& texture, Layout old_layout, Layout new_layout) {
	TODO("");
}

void Dx12GraphicsCommandList::begin_render_pass(Slice<Texture const&> attachments) {
	TODO("");
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

void Dx12GraphicsCommandList::bind_pipeline(const GraphicsPipeline& pipeline) {
	TODO("");
}

void Dx12GraphicsCommandList::bind_vertices(const Buffer& buffer) {
	TODO("");
}

void Dx12GraphicsCommandList::bind_indices(const Buffer& buffer) {
	TODO("");
}

void Dx12GraphicsCommandList::draw(usize vertex_count, usize first_vertex) {
	TODO("");
}

void Dx12GraphicsCommandList::draw_indexed(usize index_count, usize first_index) {
	TODO("");
}

void Dx12GraphicsCommandList::end_render_pass() {
	TODO("");
}

void Dx12GraphicsCommandList::end_recording() {
	throw_if_failed(m_command_list->Close());
}

void Dx12GraphicsCommandList::submit() {

}