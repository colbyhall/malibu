#include "resources.hpp"
#include "context.hpp"

#include "dx12/dx12_resources.hpp"

namespace gpu {

	Texture Texture::make(
		BitFlag<TextureUsage> usage,
		Format format,
		Vec3u32 size
	) {
		auto& context = Context::the();

		Option<SharedRef<TextureInterface>> interface;
		switch (context.backend()) {
			case Backend::Dx12:
				interface = SharedRef<TextureInterface>::make(Dx12Texture(usage, format, size));
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

		Option<SharedRef<BufferInterface>> interface;
		switch (context.backend()) {
		case Backend::Dx12:
			interface = SharedRef<BufferInterface>::make(Dx12Buffer(usage, kind, len, stride));
			break;
		}

		return Buffer { interface.unwrap() };
	}

}