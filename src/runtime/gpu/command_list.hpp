#pragma once

#include "containers/unique.hpp"
#include "resources.hpp"

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

		virtual void begin_render_pass(const Texture& color, Option<Texture const&> depth) = 0;

		virtual void set_scissor(Option<Rect2f32> scissor) = 0;
		virtual void clear_color(LinearColor color) = 0;
		virtual void clear_depth_stencil(f32 depth, u8 stencil) = 0;
		virtual void set_pipeline(const GraphicsPipeline& pipeline) = 0;
		virtual void set_vertices(const Buffer& buffer) = 0;
		virtual void set_indices(const Buffer& buffer) = 0;
		virtual void push_constant(const void* ptr) = 0;
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
		inline RenderPassRecorder& clear_color(LinearColor color) {
			m_interface.clear_color(color);
			return *this;
		}

		inline RenderPassRecorder& clear_depth_stencil(f32 depth, u8 stencil) {
			m_interface.clear_depth_stencil(depth, stencil);
			return *this;
		}

		inline RenderPassRecorder& set_pipeline(const GraphicsPipeline& pipeline) {
			m_interface.set_pipeline(pipeline);
			return *this;
		}

		inline RenderPassRecorder& set_vertices(const Buffer& buffer) {
			VERIFY(buffer.usage().is_set(BufferUsage::Vertex));
			m_interface.set_vertices(buffer);
			return *this;
		}

		inline RenderPassRecorder& set_indices(const Buffer& buffer) {
			VERIFY(buffer.usage().is_set(BufferUsage::Index));
			m_interface.set_indices(buffer);
			return *this;
		}

		inline RenderPassRecorder& push_constants(const void* ptr) {
			m_interface.push_constant(ptr);
			return *this;
		}

		inline RenderPassRecorder& draw(usize vertex_count, usize first_vertex = 0) {
			m_interface.draw(vertex_count, first_vertex);
			return *this;
		}

		inline RenderPassRecorder& draw_index(usize index_count, usize first_index = 0) {
			m_interface.draw_indexed(index_count, first_index);
			return *this;
		}

	private:
		inline RenderPassRecorder(GraphicsCommandListInterface& interface) : m_interface(interface) {}
		friend class GraphicsCommandRecorder;

		GraphicsCommandListInterface& m_interface;
	};

	class GraphicsCommandRecorder {
	public:
		inline GraphicsCommandRecorder& copy_buffer_to_texture(const Texture& dst, const Buffer& src) {
			m_interface.copy_buffer_to_texture(dst, src);
			return *this;
		}

		inline GraphicsCommandRecorder& copy_buffer_to_buffer(const Buffer& dst, const Buffer& src) {
			m_interface.copy_buffer_to_buffer(dst, src);
			return *this;
		}

		inline GraphicsCommandRecorder& texture_barrier(const Texture& texture, Layout old_layout, Layout new_layout) {
			m_interface.texture_barrier(texture, old_layout, new_layout);
			return *this;
		}

		template <typename F>
		inline GraphicsCommandRecorder& render_pass(
			const Texture& color, 
			Option<const Texture&> depth,
			F&& callable
		) {
			m_interface.begin_render_pass(color, depth);
			RenderPassRecorder recorder(m_interface);
			callable(recorder);
			m_interface.end_render_pass();
			return *this;
		}

	private:
		inline GraphicsCommandRecorder(GraphicsCommandListInterface& interface) : m_interface(interface) {}
		friend class GraphicsCommandList;

		GraphicsCommandListInterface& m_interface;
	};

	class GraphicsCommandList {
	public:
		static GraphicsCommandList make();

		template <typename F>
		inline void record(F&& callable) {
			Unique<GraphicsCommandListInterface>& interface = m_interface.as_mut().unwrap();
			interface->begin_recording();
			GraphicsCommandRecorder recorder(*interface);
			callable(recorder);
			interface->end_recording();
		}

		inline void submit() {
			Unique<GraphicsCommandListInterface>& interface = m_interface.as_mut().unwrap();
			interface->submit();
		}

	private:
		GraphicsCommandList(Unique<GraphicsCommandListInterface>&& interface) 
			: m_interface(core::move(interface)) {}

		Option<Unique<GraphicsCommandListInterface>> m_interface;
	};
}