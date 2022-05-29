#pragma once

#include "../resources.hpp"
#include "dx12_utility.hpp"

inline 
DXGI_FORMAT format_to_dxgi(gpu::Format format) {
	DXGI_FORMAT dxgi_format = DXGI_FORMAT_UNKNOWN;
	switch (format) {
		// RGB_U8,
		// RGB_U8_SRGB,
		case gpu::Format::RGBA_U8:
			dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case gpu::Format::RGBA_U8_SRGB:
			dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			break;
		case gpu::Format::RGBA_F16:
			dxgi_format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			break;
		case gpu::Format::RGBA_F32:
			dxgi_format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		// BGR_U8_SRGB,
		// Depth16,
		// Depth24_Stencil8,
	}
	return dxgi_format;
}

class Dx12Texture : public gpu::TextureInterface {
public:
	explicit Dx12Texture(
		BitFlag<gpu::TextureUsage> usage,
		gpu::Format format,
		Vec3u32 size,
		ComPtr<ID3D12Resource> resource = nullptr
	);

	BitFlag<gpu::TextureUsage> usage() const override { return m_usage; }
	gpu::Format format() const override { return m_format; }
	Vec3u32 size() const override { return m_size; }

	ComPtr<ID3D12Resource> m_resource;
	BitFlag<gpu::TextureUsage> m_usage;
	gpu::Format m_format;
	Vec3u32 m_size;

	D3D12_CPU_DESCRIPTOR_HANDLE m_rtv_handle;
};

class Dx12Buffer : public gpu::BufferInterface {
public:
	explicit Dx12Buffer(
		BitFlag<gpu::BufferUsage> usage, 
		gpu::BufferKind kind, 
		usize len, 
		usize stride
	);

	BitFlag<gpu::BufferUsage> usage() const override { return m_usage; }
	gpu::BufferKind kind() const override { return m_kind; }
	usize len() const override { return m_len; }
	usize stride() const override { return m_stride; }

	BitFlag<gpu::BufferUsage> m_usage;
	gpu::BufferKind m_kind;
	usize m_len;
	usize m_stride;

	ComPtr<ID3D12Resource> m_resource;
};