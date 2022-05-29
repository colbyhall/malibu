#pragma once

#include "core/minimal.hpp"

namespace gpu {

	class Texture;
	class Buffer;
	class GraphicsPipeline;

	enum class Layout : u8 {
		Undefined,
		General,
		ColorAttachment,
		DepthAttachment,
		TransferSrc,
		TransferDst,
		ShaderReadOnly,
		Present,
	};

	class GraphicsCommandListInterface {
	public:
		virtual void begin_recording() = 0;

		virtual void copy_buffer_to_texture(const Texture& dst, const Buffer& src) = 0;
		virtual void copy_buffer_to_buffer(const Buffer& dst, const Buffer& src) = 0;

		virtual void texture_barrier(const Texture& texture, Layout old_layout, Layout new_layout) = 0;

		virtual void begin_render_pass(Slice<Texture const&> attachments) = 0;

		virtual void set_scissor(Option<Rect2f32> scissor) = 0;
		virtual void clear_color(LinearColor color) = 0;
		virtual void clear_depth(f32 depth) = 0;
		virtual void bind_pipeline(const GraphicsPipeline& pipeline) = 0;
		virtual void bind_vertices(const Buffer& buffer) = 0;
		virtual void bind_indices(const Buffer& buffer) = 0;
		// fn bind_constants(&mut self, name: &str, buffer: &Arc<backend::Buffer>, index: usize);
		// fn bind_texture(&mut self, name: &str, texture: &Arc<backend::Texture>);
		virtual void draw(usize vertex_count, usize first_vertex) = 0;
		virtual void draw_indexed(usize index_count, usize first_index) = 0;

		virtual void end_render_pass() = 0;

		virtual void end_recording() = 0;
		virtual void submit() = 0;
		// fn submit(self, wait_on: &[Receipt]) -> Result<Arc<backend::Receipt>>;
	};

	class GraphicsCommandList {
	public:
		static GraphicsCommandList create();

	private:
		GraphicsCommandList(Unique<GraphicsCommandListInterface>&& interface) 
			: m_interface(core::move(interface)) {}

		Option<Unique<GraphicsCommandListInterface>> m_interface;
	};
}