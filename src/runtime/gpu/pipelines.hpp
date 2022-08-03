#pragma once

#include "resources.hpp"

namespace gpu {
	enum class ShaderType : u8 {
		Vertex,
		Pixel,
	};

	class ShaderInterface {
	public:
		virtual Slice<u8 const> binary() const = 0;
		virtual ShaderType type() const = 0;
	};

	enum class Primitive : u8 {
		Uint32,
		Int32,
		Float32,
		Vec2f32,
		Vec3f32,
		Vec4f32,
		Mat4f32,
	};

	struct InputParameter {
		u32 semantic_index;
		String semantic_name;
		Primitive primitive;
	};

	class VertexShaderInterface : public ShaderInterface {
	public:
		ShaderType type() const override { return ShaderType::Vertex; }
		virtual Slice<InputParameter const> input_parameters() const = 0;
	};

	class VertexShader {
	public:
		static VertexShader make(Array<u8>&& binary, Array<InputParameter>&& input_parameters);

		inline Slice<const u8> binary() const { return m_interface->binary(); }
		inline ShaderType type() const { return m_interface->type(); }
		inline Slice<InputParameter const> input_parameters() const { return m_interface->input_parameters(); }

	private:
		VertexShader(SharedRef<VertexShaderInterface>&& interface) : m_interface(core::move(interface)) { }

		SharedRef<VertexShaderInterface> m_interface;
	};

	class PixelShaderInterface : public ShaderInterface {
	public:
		ShaderType type() const override { return ShaderType::Pixel; }
	};

	class PixelShader {
	public:
		static PixelShader make(Array<u8>&& binary);

		inline Slice<const u8> binary() const { return m_interface->binary(); }
		inline ShaderType type() const { return m_interface->type(); }

	private:
		PixelShader(SharedRef<PixelShaderInterface>&& interface) : m_interface(core::move(interface)) { }

		SharedRef<PixelShaderInterface> m_interface;
	};

	enum class DrawMode : u8 {
		Fill,
		Line,
		Point
	};

	enum class CullMode : u8 {
		None,
		Front,
		Back
	};

	enum class CompareOp : u8 {
		/// `false`
		Never,
		/// `A < B`
		Less,
		/// `A == B`
		Equal,
		/// `A < B || A == B`
		LessOrEqual,
		/// `A > B`
		Greater,
		/// `A != B`
		NotEqual,
		/// `A > B || A == B`
		GreaterOrEqual,
		/// `true`
		Always,
	};

	enum class BlendOp : u8 {
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max,
	};

	enum class BlendFactor : u8 {
		Zero,
		One,

		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor,

		SrcAlpha,
		OneMinusSrcAlpha,
	};

	struct GraphicsPipelineConfig {
		VertexShader vertex_shader;
		PixelShader pixel_shader;

		Array<Format> color_attachments;
		Format depth_attachment = Format::Undefined;

		DrawMode draw_mode = DrawMode::Fill;
		f32 line_width = 1.f;
		CullMode cull_mode = CullMode::None;

		bool blend_enabled = false;

		BlendFactor src_color_blend_factor = BlendFactor::One;
		BlendFactor dst_color_blend_factor = BlendFactor::One;
		BlendOp color_blend_op = BlendOp::Add;

		BlendFactor src_alpha_blend_factor = BlendFactor::One;
		BlendFactor dst_alpha_blend_factor = BlendFactor::One;
		BlendOp alpha_blend_op = BlendOp::Add;

		bool depth_test = false;
		bool depth_write = false;
		CompareOp depth_compare = CompareOp::Always;

		inline GraphicsPipelineConfig(VertexShader&& in_vertex_shader, PixelShader&& in_pixel_shader) :
			vertex_shader(core::forward<VertexShader>(in_vertex_shader)),
			pixel_shader(core::forward<PixelShader>(in_pixel_shader)) {}
	};

	class GraphicsPipelineInterface {
	public:
		virtual const GraphicsPipelineConfig& config() const = 0;
	};

	class GraphicsPipeline {
	public:
		static GraphicsPipeline make(GraphicsPipelineConfig&& config);

		NO_DISCARD inline const GraphicsPipelineConfig& config() const { return m_interface->config(); }

		template <typename T = GraphicsPipelineInterface>
		NO_DISCARD inline T const& interface() const {
			static_assert(core::is_base_of<GraphicsPipelineInterface, T>, "T is not derived of GraphicsPipelineInterface");
			return static_cast<const T&>(*m_interface);
		}

	private:
		GraphicsPipeline(SharedRef<GraphicsPipelineInterface>&& interface) : m_interface(core::move(interface)) { }

		SharedRef<GraphicsPipelineInterface> m_interface;
	};

}