#include "asset.hpp"
#include "fs.hpp"
#include "containers/unique.hpp"

#include <cstdio>

using namespace core;

namespace asset {
	static HashMap<StringView, NonNull<AssetRegistration>> g_registration;

	AssetRegistration::AssetRegistration(StringView name) : m_name(name) {
		g_registration.insert(name, this);
	}

	struct AssetFileInfo {
		fs::MetaData meta_data;
		fs::Path path;
	};

	struct AssetEntry {
		Option<AssetFileInfo> file_info;
		Option<Unique<Asset>> asset;
	};

	static HashMap<UUID, AssetEntry> g_assets;
	void init() {

	}

	void query() {

	}
}