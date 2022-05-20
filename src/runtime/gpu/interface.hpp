#pragma once

#include "core/math/vec3.hpp"

#include "device.hpp"
#include "resources.hpp"
#include "pipelines.hpp"

namespace gpu {

	class ShaderInterface {
	public:
		virtual ShaderType type() const = 0;
		virtual Slice<u8 const> binary() const = 0;
	};

	class GraphicsPipelineInterface {
	public:
	};

	class TextureInterface {
	public:
		virtual TextureUsage usage() const = 0;
		virtual Format format() const = 0;
		virtual Vec3u32 size() const = 0;
	};

	class BufferInterface {
	public:
		virtual BufferUsage usage() const = 0;
		virtual usize len() const = 0;
	};

}