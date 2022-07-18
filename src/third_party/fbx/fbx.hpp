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

	struct SkeletonNode {
		String name;
		int parent;
		Array<usize> children;
	};

	struct Skeleton {
		Array<SkeletonNode> nodes;
		usize root;
	};

	struct Scene {
		Array<Mesh> meshes;
		Array<Skeleton> skeletons;
	};

	Result<Scene, core::fs::FileOpenError> load_fbx_scene(core::fs::PathView path);
}