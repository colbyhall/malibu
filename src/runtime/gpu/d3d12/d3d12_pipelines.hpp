#pragma once

#include "../pipelines.hpp"
#include "d3d12_utility.hpp"


class D3D12VertexShader : public gpu::VertexShaderInterface {
public:
	explicit D3D12VertexShader(Array<u8>&& binary, Array<gpu::InputParameter>&& input_parameters)
		: m_binary(core::forward<Array<u8>>(binary)), m_input_parameters(core::forward< Array<gpu::InputParameter>>(input_parameters)) {}

	Slice<u8 const> binary() const override { return m_binary; }
	Slice<gpu::InputParameter const> input_parameters() const override { return m_input_parameters; }

	Array<u8> m_binary;
	Array<gpu::InputParameter> m_input_parameters;
};

class D3D12PixelShader : public gpu::PixelShaderInterface {
public:
	explicit D3D12PixelShader(Array<u8>&& binary)
		: m_binary(core::forward<Array<u8>>(binary)) {}

	Slice<u8 const> binary() const override { return m_binary; }

	Array<u8> m_binary;
};

class D3D12GraphicsPipeline : public gpu::GraphicsPipelineInterface {
public:
	explicit D3D12GraphicsPipeline(gpu::GraphicsPipelineConfig&& config);

	const gpu::GraphicsPipelineConfig& config() const override { return m_config; }

	gpu::GraphicsPipelineConfig m_config;
	ComPtr<ID3D12PipelineState> m_pipeline;
};