#pragma once

#include "../command_list.hpp"
#include "dx12_utility.hpp"

class Dx12GraphicsCommandList : public gpu::GraphicsCommandListInterface {
public:
	explicit Dx12GraphicsCommandList();

	void begin_recording() override;
	void copy_buffer_to_texture(const Texture& dst, const Buffer& src) override;
	void copy_buffer_to_buffer(const Buffer& dst, const Buffer& src) override;
	void texture_barrier(const Texture& texture, Layout old_layout, Layout new_layout) override;
	void begin_render_pass(Slice<Texture const&> attachments) override;
	void set_scissor(Option<Rect2f32> scissor) override;
	void clear_color(LinearColor color) override;
	void clear_depth(f32 depth) override;
	void bind_pipeline(const GraphicsPipeline& pipeline) override;
	void bind_vertices(const Buffer& buffer) override;
	void bind_indices(const Buffer& buffer) override;
	// fn bind_constants(&mut self, name: &str, buffer: &Arc<backend::Buffer>, index: usize);
	// fn bind_texture(&mut self, name: &str, texture: &Arc<backend::Texture>);
	void draw(usize vertex_count, usize first_vertex) override;
	void draw_indexed(usize index_count, usize first_index) override;
	void end_render_pass() override;
	void end_recording() override;
	void submit() override;

private:
	ComPtr<ID3D12GraphicsCommandList> m_command_list;
};