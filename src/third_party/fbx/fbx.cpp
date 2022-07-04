#include "fbx.hpp"
#include "core.hpp"

#define FBXSDK_SHARED
#include "fbxsdk.h"

#include <cstdio>

namespace fbx {
	class Context {
	public:
		explicit Context() {
			manager = FbxManager::Create();
			VERIFY(manager);
			ios = FbxIOSettings::Create(manager, "test");
		}

		static Context& the() {
			static Context context;
			return context;
		}

		FbxManager* manager;
		FbxIOSettings* ios;
	};

    Result<Mesh, core::fs::FileOpenError> load_mesh(core::fs::PathView path) {
		auto& context = Context::the();

		int sdk_maj, sdk_min, sdk_rev;
		FbxManager::GetFileFormatVersion(sdk_maj, sdk_min, sdk_rev);

		FbxImporter* const importer = FbxImporter::Create(context.manager, "");
		const bool import_status = importer->Initialize(path.ptr(), -1, context.ios);
        if (!import_status || !importer->IsFBX()) return core::fs::FileOpenError::NotFound;

		FbxScene* const scene = FbxScene::Create(context.manager, "Scene");
		const bool status = importer->Import(scene);
		VERIFY(status);

        Array<Vertex> vertices;
        Array<u32> indices;
        for (int i = 0; i < scene->GetNodeCount(); ++i) {
            auto* const node = scene->GetNode(i);
            auto* const mesh = node->GetMesh();
            if (!mesh) continue;

            for (int j = 0; j < mesh->GetPolygonCount(); ++j) {
                const auto size = mesh->GetPolygonSize(j);

                // FIXME: Proper polygon triangulation
                if (size == 3) {
					const auto start = vertices.len();
                    for (int k = 0; k < size; ++k) {
                        const auto vertex_index = mesh->GetPolygonVertex(j, k);
                        const auto control_point = mesh->GetControlPointAt(vertex_index);
                        FbxVector4 normal;
						
                        // const bool found = mesh->GetPolygonVertexNormal(j, vertex_index, normal);
                        indices.push((u32)vertices.len());
                        vertices.push(Vertex {
                            { 
								(f32)control_point[0], 
								(f32)control_point[1], 
								(f32)control_point[2] 
							},
                            { 
								(f32)normal[0], 
								(f32)normal[1], 
								(f32)normal[2] 
							},
                        });
                        // TODO: UV's
                    }

					// HACK: Fix to get normals in temoporarily
					const auto a = vertices[start + 0].position;
					const auto b = vertices[start + 1].position;
					const auto c = vertices[start + 2].position;

					const auto a_to_b = b - a;
					const auto a_to_c = c - a;

					const auto normal = a_to_c.cross(a_to_b);
					for (int k = 0; k < size; ++k) {
						vertices[start + k].normal = normal;
					}
                } else if (size == 4) {
                    const u32 start = (u32)vertices.len();
                    for (int k = 0; k < size; ++k) {
                        const auto vertex_index = mesh->GetPolygonVertex(j, k);
                        const auto control_point = mesh->GetControlPointAt(vertex_index);
                        FbxVector4 normal;
                        mesh->GetPolygonVertexNormal(j, vertex_index, normal);
                        vertices.push(Vertex {
                                { 
									(f32)control_point[0], 
									(f32)control_point[1], 
									(f32)control_point[2] 
								},
                                { 
									(f32)normal[0], 
									(f32)normal[1], 
									(f32)normal[2] 
								},
                        });
                        // TODO: UV's
                    }

                    // First Triangle
                    indices.push(start + 0);
                    indices.push(start + 1);
                    indices.push(start + 2);

                    // Second Triangle
                    indices.push(start + 0);
                    indices.push(start + 2);
                    indices.push(start + 3);

					const auto a = vertices[start + 0].position;
					const auto b = vertices[start + 1].position;
					const auto c = vertices[start + 2].position;

					const auto a_to_b = b - a;
					const auto a_to_c = c - a;

					const auto normal = a_to_c.cross(a_to_b);
					for (int k = 0; k < size; ++k) {
						vertices[start + k].normal = normal;
					}
                }
            }
        }

        return Mesh { core::move(vertices), core::move(indices) };
	}
}