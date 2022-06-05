#pragma once

#include "resources.hpp"
#include "core/minimal.hpp"

namespace gpu {

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

	class RenderPassRecorder {
	public:
		ALWAYS_INLINE RenderPassRecorder& clear_color(LinearColor color) {
			m_interface.clear_color(color);
			return *this;
		}

		ALWAYS_INLINE RenderPassRecorder& bind_pipeline(const GraphicsPipeline& pipeline) {
			m_interface.bind_pipeline(pipeline);
			return *this;
		}

		ALWAYS_INLINE RenderPassRecorder& bind_vertices(const Buffer& buffer) {
			VERIFY(buffer.usage().is_set(BufferUsage::Vertex));
			m_interface.bind_vertices(buffer);
			return *this;
		}

		ALWAYS_INLINE RenderPassRecorder& draw(usize vertex_count, usize first_vertex = 0) {
			m_interface.draw(vertex_count, first_vertex);
			return *this;
		}

	private:
		ALWAYS_INLINE RenderPassRecorder(GraphicsCommandListInterface& interface) : m_interface(interface) {}
		friend class GraphicsCommandRecorder;

		GraphicsCommandListInterface& m_interface;
	};

	class GraphicsCommandRecorder {
	public:
		template <typename Callable>
		ALWAYS_INLINE GraphicsCommandRecorder& render_pass(Slice<Texture const&> attachments, Callable&& callable) {
			m_interface.begin_render_pass(attachments);
			RenderPassRecorder recorder(m_interface);
			callable(recorder);
			m_interface.end_render_pass();
			return *this;
		}

	private:
		ALWAYS_INLINE GraphicsRecorder(GraphicsCommandListInterface& interface) : m_interface(interface) {}
		friend class GraphicsCommandList;

		GraphicsCommandListInterface& m_interface;
	};

	class GraphicsCommandList {
	public:
		static GraphicsCommandList make();

		template <typename Callable>
		ALWAYS_INLINE void record(Callable&& callable) {
			Unique<GraphicsCommandListInterface>& interface = m_interface.as_ref().unwrap()
			interface->begin_recording();
			GraphicsCommandRecorder recorder(*interface);
			callable(recorder);
			interface->end_recording();
		}

	private:
		GraphicsCommandList(Unique<GraphicsCommandListInterface>&& interface) 
			: m_interface(core::move(interface)) {}

		Option<Unique<GraphicsCommandListInterface>> m_interface;
	};
}