#pragma once

#include "core/containers/array.hpp"
#include "core/containers/bitflag.hpp"
#include "core/containers/shared_ref.hpp"

#include "resources.hpp"

namespace gpu {

	enum class ShaderType : u8 {
		Vertex,
		Pixel,
	};

	class ShaderInterface {
	public:
		virtual Slice<const u8> binary() const = 0;
	};

	class Shader {
	public:
		ALWAYS_INLINE 
		Slice<const u8> binary() const { return m_interface->binary(); }

	private:
		SharedRef<ShaderInterface> m_interface;
	};

	enum class DrawMode : u8 {
		Fill,
		Line,
		Point
	};

	enum class CullMode : u8 {
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

	enum class Primitive : u8 {
		Uint32,
		Int32,
		Float32,
		Vec2f32,
		Vec3f32,
		Vec4f32,
		Mat4f32,
	};

	struct GraphicsPipelineConfig {
		Array<Format> attachments;
		Array<Shader> shaders;
		Array<Primitive> vertex_primitives;

		DrawMode draw_mode = DrawMode::Fill;
		f32 line_width = 1.f;
		BitFlag<CullMode> cull_mode = CullMode::Back;

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
	};

}