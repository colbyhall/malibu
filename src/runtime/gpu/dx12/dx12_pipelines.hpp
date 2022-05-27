#pragma once

#include "../pipelines.hpp"
#include "dx12_utility.hpp"

class Dx12Shader : public gpu::ShaderInterface {
public:
	explicit Dx12Shader(StringView source, gpu::ShaderType type);

	Slice<u8 const> binary() const override { return {}; }
	gpu::ShaderType type() const override { return m_type; }

private:
	ComPtr<ID3DBlob> m_blob;
	gpu::ShaderType m_type;
};

class Dx12GraphicsPipeline : public gpu::GraphicsPipelineInterface {
public:
	explicit Dx12GraphicsPipeline(gpu::GraphicsPipelineConfig&& config);

	const gpu::GraphicsPipelineConfig& config() const override { return m_config; }

private:
	gpu::GraphicsPipelineConfig m_config;
	ComPtr<ID3D12PipelineState> m_pipeline;
};