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

	enum class BufferKind {
		Storage,
		Upload,
		Download,
	};

	class BufferInterface {
	public:
		virtual BitFlag<BufferUsage> usage() const = 0;
		virtual BufferKind kind() const = 0;
		virtual usize len() const = 0;
		virtual usize stride() const = 0;
	};

	class Buffer {
	public:
		static Buffer create(
			BitFlag<gpu::BufferUsage> usage,
			gpu::BufferKind kind,
			usize len,
			usize stride
		);
	private:
		Buffer(SharedRef<BufferInterface>&& interface) : m_interface(core::move(interface)) { }

		SharedRef<BufferInterface> m_interface;
	};
}