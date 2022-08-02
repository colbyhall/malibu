#pragma once

#include "core.hpp"
#include "pipelines.hpp"

namespace dxc {
	using gpu::ShaderType;

	struct Input {
		StringView source;
		StringView main;
		ShaderType type;
	};

	Result<Array<u8>, String> compile(const Input& input);
}