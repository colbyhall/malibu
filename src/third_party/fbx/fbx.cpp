#include "fbx.hpp"
#include "core.hpp"

#define FBXSDK_SHARED
#include "fbxsdk.h"

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

    Result<Scene, core::fs::FileOpenError> load_fbx_scene(core::fs::PathView path) {
		auto& context = Context::the();

		int sdk_maj, sdk_min, sdk_rev;
		FbxManager::GetFileFormatVersion(sdk_maj, sdk_min, sdk_rev);

		FbxImporter* const importer = FbxImporter::Create(context.manager, "");
		const bool import_status = importer->Initialize(path.ptr(), -1, context.ios);
        if (!import_status || !importer->IsFBX()) return core::fs::FileOpenError::NotFound;

		FbxScene* const scene = FbxScene::Create(context.manager, "Scene");
		const bool status = importer->Import(scene);
		VERIFY(status);

		Scene result = {};

		for (
			int geometry_index = 0;
			geometry_index < scene->GetGeometryCount();
			++geometry_index
		) {
			auto* geometry = scene->GetGeometry(geometry_index);
			if (geometry->GetAttributeType() == FbxNodeAttribute::eMesh) {
				auto* geometry_node = geometry->GetNode();
				auto* mesh = (FbxMesh*)geometry;

				if (!mesh->IsTriangleMesh()) {
					// TODO: Error message
					continue;
				}

				Array<Vertex> vertices;
				vertices.reserve(mesh->GetControlPointsCount());
				Array<u32> indices;
				indices.reserve(mesh->GetPolygonCount() * 3);

#if 0
				const auto num_stacks = scene->GetSrcObjectCount<FbxAnimStack>();
				if (num_stacks > 0) {
					auto* stack = scene->GetCurrentAnimationStack();

					const auto num_layers = stack->GetMemberCount<FbxAnimLayer>();
					if (num_layers > 0) {
						auto* layer = stack->GetMember<FbxAnimLayer>(0);
						auto curve = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
					}
				}
#endif

				auto* layer = mesh->GetLayer(0);
				
				auto* normal_element = layer->GetNormals();
				FbxLayerElement::EReferenceMode normal_reference_mode = FbxLayerElement::eDirect;
				FbxLayerElement::EMappingMode normal_mapping_mode = FbxLayerElement::eByControlPoint;
				if (normal_element) {
					normal_reference_mode = normal_element->GetReferenceMode();
					normal_mapping_mode = normal_element->GetMappingMode();
				}

				for (
					int polygon_index = 0; 
					polygon_index < mesh->GetPolygonCount(); 
					++polygon_index
				) {
					const auto polygon_size = mesh->GetPolygonSize(polygon_index);
					VERIFY(polygon_size == 3);

					for (int vertex_index = 0; vertex_index < polygon_size; ++vertex_index) {
						const auto control_point_index = mesh->GetPolygonVertex(polygon_index, vertex_index);
						const auto control_point = mesh->GetControlPointAt(control_point_index);

						// Lookup the normals
						Vec3f32 normal = {};
						if (normal_element) {
							const int normal_map_index = (normal_mapping_mode == FbxLayerElement::eByControlPoint) ?
								control_point_index : (int)vertices.len();
							const int normal_value_index = (normal_reference_mode == FbxLayerElement::eDirect) ?
								normal_map_index : normal_element->GetIndexArray().GetAt(normal_map_index);

							const auto value = normal_element->GetDirectArray().GetAt(normal_value_index);
							normal.x = (f32)value[0];
							normal.y = (f32)value[1];
							normal.z = (f32)value[2];
						}

						indices.push((u32)vertices.len());
						vertices.push(Vertex{
							{
								(f32)control_point[0],
								(f32)control_point[1],
								(f32)control_point[2]
							},
							normal
						});
						// TODO: UV's
					}
				}

				// TODO: Import the skeletal mesh
				if (mesh->GetDeformerCount(FbxDeformer::eSkin) > 0) {
					auto* skin = (FbxSkin*)mesh->GetDeformer(0, FbxDeformer::eSkin);

					FbxNode* root = nullptr;
					for (
						int cluster_index = 0;
						cluster_index < skin->GetClusterCount();
						++cluster_index
					) {
						auto* cluster = skin->GetCluster(cluster_index);
						root = cluster->GetLink();
						while (
							root && 
							root->GetParent() && 
							root->GetParent()->GetSkeleton()
						) root = root->GetParent();

						if (root) break;
					}

					auto position0 = root->GetGeometricTranslation(FbxNode::eSourcePivot);
					auto rotation0 = root->GetGeometricRotation(FbxNode::eSourcePivot);

					for (
						int child_index = 0;
						child_index < root->GetChildCount();
						++child_index
					) {
						auto* child = root->GetChild(child_index);
						
						auto position1 = child->GetGeometricTranslation(FbxNode::eSourcePivot);
						auto rotation1 = child->GetGeometricRotation(FbxNode::eSourcePivot);
					}

				}

				result.meshes.push(Mesh{
					String("None"),
					core::move(vertices),
					core::move(indices),

					-1,
				});
			}
		}

        return result;
	}
}