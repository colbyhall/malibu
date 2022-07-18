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

		// Create the scene and import the file into it
		FbxScene* const scene = FbxScene::Create(context.manager, "Scene");
		const bool status = importer->Import(scene);
		VERIFY(status);

		// Convert scene to use all of our coordinate systems
		FbxAxisSystem::ECoordSystem coord_system = FbxAxisSystem::eRightHanded;
		FbxAxisSystem::EUpVector up_vector = FbxAxisSystem::eZAxis;
		FbxAxisSystem::EFrontVector front_vector = (FbxAxisSystem::EFrontVector)-FbxAxisSystem::eParityOdd;

		FbxAxisSystem adjusted_axis_system = { up_vector, front_vector, coord_system };
		adjusted_axis_system.ConvertScene(scene);

		Scene result = {};

		const auto geometry_count = scene->GetGeometryCount();
		for (
			int geometry_index = 0;
			geometry_index < geometry_count;
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

				Mesh output = {};
				
				output.vertices.reserve(mesh->GetControlPointsCount());
				output.indices.reserve(mesh->GetPolygonCount() * 3);

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
								control_point_index : (int)output.vertices.len();
							const int normal_value_index = (normal_reference_mode == FbxLayerElement::eDirect) ?
								normal_map_index : normal_element->GetIndexArray().GetAt(normal_map_index);

							const auto value = normal_element->GetDirectArray().GetAt(normal_value_index);
							normal.x = (f32)value[0];
							normal.y = (f32)value[1];
							normal.z = (f32)value[2];

							VERIFY(normal.len_sq() != 0.f);
						}

						output.indices.push((u32)output.vertices.len());
						output.vertices.push(Vertex{
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

					Skeleton skeleton = {};
					skeleton.bones.reserve(256); // FIXME: Make average number of bones

					// Find the root node to start traversing
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

					struct FbxBone {
						int parent;
						FbxNode* node;
					};
					// FIXME: Use a Queue here
					Array<FbxBone> fbx_bones;
					fbx_bones.reserve(16);

					// Add the root node which has no parent
					fbx_bones.push(FbxBone{
						-1, 
						root
					});

					auto* pose = scene->GetPose(0);

					while (fbx_bones.len() > 0) {
						auto fbx_bone = fbx_bones.remove(0);

						auto fbx_name = fbx_bone.node->GetNameOnly();
						StringView name = fbx_name;

						const auto pose_index = pose->Find(fbx_bone.node);
						if (pose_index == -1) continue;

						const auto non_affine_matrix = pose->GetMatrix(pose_index);
						const auto unconverted_matrix = *(FbxAMatrix*)(double*)&non_affine_matrix;
						
						FbxAMatrix correction_matrix;
						correction_matrix.SetIdentity();
						correction_matrix.SetR(FbxVector4(-90.0, -90.0, 0.0));
						
						const auto matrix = unconverted_matrix * correction_matrix;
						const auto position = matrix.GetT();
						const auto rotation = matrix.GetQ();
						const auto scale = matrix.GetS();

						const auto bone_index = skeleton.bones.len();
						skeleton.bones.push(Bone{
							String::from(name),

							fbx_bone.parent,
							{},

							Vec3f32 { (f32)position[0], (f32)position[1], (f32)position[2] },
							Quatf32 { (f32)rotation[0], (f32)rotation[1], (f32)rotation[2], (f32)rotation[3] },
							Vec3f32 { (f32)scale[0], (f32)scale[1], (f32)scale[2] },
						});

						// Go to the parent and add the child index
						if (fbx_bone.parent != -1) {
							skeleton.bones[fbx_bone.parent].children.push(bone_index);
						}

						for (
							int child_index = 0;
							child_index < fbx_bone.node->GetChildCount();
							++child_index
							) {
							auto* child = fbx_bone.node->GetChild(child_index);

							if (child->GetSkeleton()) {
								fbx_bones.push(FbxBone{
									(int)bone_index,
									child,
								});
							}
						}
					}

					const auto skeleton_index = result.skeletons.len();
					result.skeletons.push(core::move(skeleton));
					output.skeleton = (int)skeleton_index;
				}

				result.meshes.push(core::move(output));
			}
		}


		return result;
	}
}