#pragma once

#include "../pipelines.hpp"
#include "dx12_utility.hpp"

class Dx12Shader : public gpu::ShaderInterface {
public:
private:
	ComPtr<ID3DBlob> m_blob;
};

class Dx12GraphicsPipeline : public gpu::GraphicsPipelineInterface {
public:
	explicit Dx12GraphicsPipeline(gpu::GraphicsPipelineConfig&& config);
private:
	gpu::GraphicsPipelineConfig m_config;
	ComPtr<ID3D12PipelineState> m_pipeline;
};