#pragma once

#include "core.hpp"
#include "containers/bitflag.hpp"
#include "containers/shared_ref.hpp"
#include "math.hpp"

class D3D12Swapchain;

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
		ColorAttachment,
		DepthAttachment,
		SwapChain,
	};

	enum class Format : u16{
		Undefined,
		
		R_U8,

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

	usize format_size_in_bytes(Format format);

	class TextureInterface {
	public:
		virtual BitFlag<TextureUsage> usage() const = 0;
		virtual Format format() const = 0;
		virtual Vec3u32 size() const = 0;
		virtual u32 bindless() const = 0;
		virtual ~TextureInterface() {}
	};

	class Texture {
	public:
		static Texture make(
			BitFlag<TextureUsage> usage,
			Format format,
			Vec3u32 size
		);

		NO_DISCARD inline BitFlag<TextureUsage> usage() const { return m_interface->usage(); }
		NO_DISCARD inline Format format() const { return m_interface->format(); }
		NO_DISCARD inline Vec3u32 size() const { return m_interface->size(); }
		NO_DISCARD inline u32 bindless() const { return m_interface->bindless(); }

		template <typename T = TextureInterface>
		NO_DISCARD inline T const& interface() const {
			static_assert(core::is_base_of<TextureInterface, T>, "T is not derived of TextureInterface");
			return static_cast<const T&>(*m_interface);
		}

		inline Texture clone() const { return m_interface.clone(); }

	private:
		Texture(AtomicSharedRef<TextureInterface>&& interface) : m_interface(core::move(interface)) { }

		friend D3D12Swapchain;

		AtomicSharedRef<TextureInterface> m_interface;
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
		virtual Slice<u8> write() const = 0;
		// virtual Slice<u8 const> read() const = 0;
		virtual void unmap() const = 0;
		virtual ~BufferInterface() {}
	};

	class Buffer {
	public:
		static Buffer make(
			BitFlag<gpu::BufferUsage> usage,
			gpu::BufferKind kind,
			usize len,
			usize stride
		);

		NO_DISCARD inline BitFlag<BufferUsage> usage() const { return m_interface->usage(); }
		NO_DISCARD inline BufferKind kind() const { return m_interface->kind(); }
		NO_DISCARD inline usize len() const { return m_interface->len(); }
		NO_DISCARD inline usize stride() const { return m_interface->stride(); }

		inline Buffer clone() const { return m_interface.clone(); }

		template <typename F>
		void write(F&& callable) {
			Slice<u8> slice = m_interface->write();
			callable(slice);
			m_interface->unmap();
		}

		template <typename T = BufferInterface>
		T const& interface() const { 
			static_assert(core::is_base_of<BufferInterface, T>, "T is not derived of BufferInterface");
			return static_cast<const T&>(*m_interface);
		}

	private:
		Buffer(AtomicSharedRef<BufferInterface>&& interface) : m_interface(core::move(interface)) { }

		AtomicSharedRef<BufferInterface> m_interface;
	};
}