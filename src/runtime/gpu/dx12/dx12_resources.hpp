#pragma once

#include "../resources.hpp"
#include "dx12_utility.hpp"

class Dx12Texture : public gpu::TextureInterface {
public:

private:
	ComPtr<ID3D12Resource> m_resource;
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