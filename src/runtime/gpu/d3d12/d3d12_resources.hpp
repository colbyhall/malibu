#pragma once

#include "../resources.hpp"
#include "d3d12_utility.hpp"

inline DXGI_FORMAT format_to_dxgi(gpu::Format format) {
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
		case gpu::Format::Depth24_Stencil8:
			dxgi_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		default:
			TODO("Unsupported format type");
			break;
	}

	return dxgi_format;
}

class D3D12Texture : public gpu::TextureInterface {
public:
	explicit D3D12Texture(
		BitFlag<gpu::TextureUsage> usage,
		gpu::Format format,
		Vec3u32 size,
		ComPtr<ID3D12Resource> resource = nullptr
	);

	NO_COPY(D3D12Texture);
	inline D3D12Texture(D3D12Texture&& move) noexcept : 
		m_resource(core::move(move.m_resource)),
		m_usage(move.m_usage),
		m_format(move.m_format),
		m_size(move.m_size),
		m_rtv_handle(move.m_rtv_handle),
		m_dsv_handle(move.m_dsv_handle)
	{
		move.m_resource = nullptr;
	}

	BitFlag<gpu::TextureUsage> usage() const override { return m_usage; }
	gpu::Format format() const override { return m_format; }
	Vec3u32 size() const override { return m_size; }

	ComPtr<ID3D12Resource> m_resource;
	BitFlag<gpu::TextureUsage> m_usage;
	gpu::Format m_format;
	Vec3u32 m_size;

	D3D12_CPU_DESCRIPTOR_HANDLE m_rtv_handle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_dsv_handle;
};

class D3D12Buffer : public gpu::BufferInterface {
public:
	explicit D3D12Buffer(
		BitFlag<gpu::BufferUsage> usage, 
		gpu::BufferKind kind, 
		usize len, 
		usize stride
	);

	NO_COPY(D3D12Buffer);
	inline D3D12Buffer(D3D12Buffer&& move) noexcept :
		m_resource(core::move(move.m_resource)),
		m_usage(move.m_usage),
		m_kind(move.m_kind),
		m_len(move.m_len),
		m_stride(move.m_stride)
	{
		move.m_resource = nullptr;
	}

	BitFlag<gpu::BufferUsage> usage() const override { return m_usage; }
	gpu::BufferKind kind() const override { return m_kind; }
	usize len() const override { return m_len; }
	usize stride() const override { return m_stride; }
	Slice<u8> write() const override;
	void unmap() const override;

	BitFlag<gpu::BufferUsage> m_usage;
	gpu::BufferKind m_kind;
	usize m_len;
	usize m_stride;

	ComPtr<ID3D12Resource> m_resource;
};