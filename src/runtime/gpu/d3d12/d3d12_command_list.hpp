#pragma once

#include "../command_list.hpp"
#include "d3d12_resources.hpp"

inline D3D12_RESOURCE_STATES layout_to_resource_states(gpu::Layout layout) {
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
	switch (layout) {
		case gpu::Layout::Undefined:
			break;
		case gpu::Layout::General:
			state = D3D12_RESOURCE_STATE_GENERIC_READ;
			break;
		case gpu::Layout::ColorAttachment:
			state = D3D12_RESOURCE_STATE_RENDER_TARGET;
			break;
		// case gpu::Layout::DepthAttachment:
		// case gpu::Layout::TransferSrc:
		case gpu::Layout::TransferDst:
			state = D3D12_RESOURCE_STATE_COPY_DEST;
			break;
		// case gpu::Layout::ShaderReadOnly:
		case gpu::Layout::Present:
			state = D3D12_RESOURCE_STATE_PRESENT;
			break;
		default:
			TODO("");
			break;
	}
	return state;
}

class D3D12GraphicsCommandList : public gpu::GraphicsCommandListInterface {
public:
	explicit D3D12GraphicsCommandList();

	void begin_recording() override;
	void copy_buffer_to_texture(const gpu::Texture& dst, const gpu::Buffer& src) override;
	void copy_buffer_to_buffer(const gpu::Buffer& dst, const gpu::Buffer& src) override;
	void texture_barrier(const gpu::Texture& texture, gpu::Layout old_layout, gpu::Layout new_layout) override;
	void begin_render_pass(const gpu::Texture& color, Option<const gpu::Texture&> depth) override;
	void set_scissor(Option<Rect2f32> scissor) override;
	void clear_color(LinearColor color) override;
	void clear_depth_stencil(f32 depth, u8 stencil) override;
	void set_pipeline(const gpu::GraphicsPipeline& pipeline) override;
	void set_vertices(const gpu::Buffer& buffer) override;
	void set_indices(const gpu::Buffer& buffer) override;
	void push_constant(const void* ptr) override;
	// fn bind_constants(&mut self, name: &str, buffer: &Arc<backend::Buffer>, index: usize);
	// fn bind_texture(&mut self, name: &str, texture: &Arc<backend::Texture>);
	void draw(usize vertex_count, usize first_vertex) override;
	void draw_indexed(usize index_count, usize first_index) override;
	void end_render_pass() override;
	void end_recording() override;
	void submit() override;

private:
	ComPtr<ID3D12GraphicsCommandList> m_command_list;

	Option<D3D12_CPU_DESCRIPTOR_HANDLE> m_bound_color_buffer;
	Option<D3D12_CPU_DESCRIPTOR_HANDLE> m_bound_depth_buffer;

	Array<gpu::Texture> m_textures_in_use;
	Array<gpu::Buffer> m_buffers_in_use;
};