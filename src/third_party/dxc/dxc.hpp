#pragma once

#include "core.hpp"
#include "pipelines.hpp"

namespace dxc {
	using gpu::ShaderType;
	using gpu::InputParameter;

	struct Input {
		StringView source;
		StringView main;
		ShaderType type;
	};

	struct Output {
		Array<u8> binary;
		Array<InputParameter> input_parameters;
	};

	Result<Output, String> compile(const Input& input);
}