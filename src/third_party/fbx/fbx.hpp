#pragma once

#include "math.hpp"
#include "fs.hpp"

namespace fbx {
    struct Vertex {
        Vec3f32 position;
        Vec3f32 normal;
    };

    struct Mesh {
        Array<Vertex> vertices;
        Array<u32> indices;
    };

	Result<Mesh, core::fs::FileOpenError> load_mesh(core::fs::PathView path);
}