#pragma once

#include "primitives.hpp"

namespace gpu {

	enum class Filter : u8 {
		Nearest,
		Linear
	};

	enum class Wrap : u8 {
		Clamp,
		Repeat,
	};

	struct Sampler {
		Filter min_filter = Filter::Linear;
		Filter mag_filter = Filter::Linear;
		Wrap address_u = Wrap::Clamp;
		Wrap address_v = Wrap::Clamp;
		Wrap address_w = Wrap::Clamp;
	};

	enum class TextureUsage : u8 {
		TransferSrc,
		TransferDst,
		Sampled,
		Color_Attachment,
		Depth_Attachment,
		SwapChain,
	};

	enum class Format : u16{
		Undefined,

		RGB_U8,
		RGB_U8_SRGB,
		RGBA_U8,
		RGBA_U8_SRGB,

		RGBA_F16,

		RGBA_F32,

		BGR_U8_SRGB,

		Depth16,
		Depth24_Stencil8,
	};

	enum class BufferUsage : u8 {
		TransferSrc,
		TransferDst,
		Vertex,
		Index,
		Constant,
	};

}