#include "resources.hpp"
#include "context.hpp"

#include "d3d12/d3d12_resources.hpp"

namespace gpu {
	usize format_size_in_bytes(Format format) {
		switch (format) {
		case gpu::Format::R_U8:
			return 1;
			break;
			// RGB_U8,
			// RGB_U8_SRGB,
		case gpu::Format::RGBA_U8:
		case gpu::Format::RGBA_U8_SRGB:
			return 4;
			break;
		case gpu::Format::RGBA_F16:
			return 4 * 2;
			break;
		case gpu::Format::RGBA_F32:
			return 4 * 4;
			break;
			// BGR_U8_SRGB,
			// Depth16,
		case gpu::Format::Depth24_Stencil8:
			return 4;
			break;
		default:
			TODO("Unsupported format type");
			break;
		}

		return 0;
	}

	Texture Texture::make(
		BitFlag<TextureUsage> usage,
		Format format,
		Vec3u32 size
	) {
		auto& context = Context::the();

		Option<AtomicSharedRef<TextureInterface>> interface;
		switch (context.backend()) {
			case Backend::D3D12:
				interface = AtomicSharedRef<TextureInterface>::make<D3D12Texture>(
					usage, 
					format, 
					size
				);
				break;
		}

		return Texture { interface.unwrap() };
	}

	Buffer Buffer::make(
		BitFlag<gpu::BufferUsage> usage,
		gpu::BufferKind kind,
		usize len,
		usize stride
	) {
		auto& context = Context::the();

		Option<AtomicSharedRef<BufferInterface>> interface;
		switch (context.backend()) {
		case Backend::D3D12:
			interface = AtomicSharedRef<BufferInterface>::make<D3D12Buffer>(
				usage, 
				kind, 
				len, 
				stride
			);
			break;
		}

		return Buffer { interface.unwrap() };
	}
}