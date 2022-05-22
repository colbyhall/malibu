#pragma once

#include "core/minimal.hpp"

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

	class TextureInterface {
	public:
		virtual BitFlag<TextureUsage> usage() const = 0;
		virtual Format format() const = 0;
		virtual Vec3u32 size() const = 0;
	};

	class Texture {
	public:
	private:
		SharedRef<TextureInterface> m_interface;
	};

	enum class BufferUsage : u8 {
		TransferSrc,
		TransferDst,
		Vertex,
		Index,
		Constant,
	};

	class BufferInterface {
	public:
	};
}