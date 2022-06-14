#include "fbx.hpp"
#include "core.hpp"

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

	void load_mesh(core::fs::PathView path) {
		auto& context = Context::the();

		int sdk_maj, sdk_min, sdk_rev;
		FbxManager::GetFileFormatVersion(sdk_maj, sdk_min, sdk_rev);

		FbxImporter* importer = FbxImporter::Create(context.manager, "");
		const bool import_status = importer->Initialize(path.ptr(), -1, context.ios);
		VERIFY(import_status && importer->IsFBX());

		FbxScene* scene = FbxScene::Create(context.manager, "Scene");
		const bool status = importer->Import(scene);
		VERIFY(status);

        for (int i = 0; i < scene->GetNodeCount(); ++i) {
            auto* node = scene->GetNode(i);
            auto* attribute = node->GetNodeAttribute();

            if (attribute == nullptr) continue;
            if (attribute->GetAttributeType() != FbxNodeAttribute::eMesh) continue;

            auto* mesh = (FbxMesh*)attribute;


        }
	}
}