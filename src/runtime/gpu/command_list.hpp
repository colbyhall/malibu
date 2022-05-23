#pragma once

#include "pipelines.hpp"
#include "resources.hpp"

namespace gpu {
	class GraphicsCommandListInterface {
	public:
		virtual void begin_recording();

		virtual void copy_buffer_to_texture(const Texture& dst, const Buffer& src);
		virtual void copy_buffer_to_buffer(const Buffer& dst, const Buffer& src);

		virtual void texture_barrier(const Texture& texture, Layout old_layout, Layout new_layout);

		virtual void begin_render_pass(Slice<Texture const&> attachments);

		virtual void set_scissor(Option<Rect2f32> scissor);
		virtual void clear_color(LinearColor color);
		virtual void clear_depth(f32 depth);
		virtual void bind_pipeline(const GraphicsPipeline& pipeline);
		virtual void bind_vertices(const Buffer& buffer);
		virtual void bind_indices(const Buffer& buffer);
		// fn bind_constants(&mut self, name: &str, buffer: &Arc<backend::Buffer>, index: usize);
		// fn bind_texture(&mut self, name: &str, texture: &Arc<backend::Texture>);
		virtual void draw(usize vertex_count, usize first_vertex);
		virtual void draw_indexed(usize index_count, usize first_index);

		virtual void end_render_pass();

		virtual void end_recording();

		// fn submit(self, wait_on: &[Receipt]) -> Result<Arc<backend::Receipt>>;
	};
}