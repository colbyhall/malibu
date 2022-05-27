#pragma once

#include "../resources.hpp"
#include "dx12_utility.hpp"

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

private:
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

private:
	BitFlag<gpu::BufferUsage> m_usage;
	gpu::BufferKind m_kind;
	usize m_len;
	usize m_stride;

	ComPtr<ID3D12Resource> m_resource;
};