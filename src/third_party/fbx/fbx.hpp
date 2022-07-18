#pragma once

#include "math.hpp"
#include "fs.hpp"

namespace fbx {
	struct Vertex {
		Vec3f32 position;
		Vec3f32 normal;
	};

	struct Mesh {
		String name;
		Array<Vertex> vertices;
		Array<u32> indices;

		int skeleton = -1;
	};

	struct Bone {
		String name;

		int parent;
		Array<usize> children;

		Vec3f32 position;
		Quatf32 rotation;
		Vec3f32 scale;
	};

	struct Skeleton {
		Array<Bone> bones;
	};

	struct Scene {
		Array<Mesh> meshes;
		Array<Skeleton> skeletons;
	};

	Result<Scene, core::fs::FileOpenError> load_fbx_scene(core::fs::PathView path);
}