#pragma once

#include "../pipelines.hpp"
#include "d3d12_utility.hpp"

class D3D12Shader : public gpu::ShaderInterface {
public:
	explicit D3D12Shader(Array<u8>&& binary, gpu::ShaderType type)
	: m_binary(core::forward<Array<u8>>(binary)), m_type(type) {}

	Slice<u8 const> binary() const override { return m_binary; }
	gpu::ShaderType type() const override { return m_type; }

	Array<u8> m_binary;
	gpu::ShaderType m_type;
};

class D3D12GraphicsPipeline : public gpu::GraphicsPipelineInterface {
public:
	explicit D3D12GraphicsPipeline(gpu::GraphicsPipelineConfig&& config);

	const gpu::GraphicsPipelineConfig& config() const override { return m_config; }

	gpu::GraphicsPipelineConfig m_config;
	ComPtr<ID3D12PipelineState> m_pipeline;
};