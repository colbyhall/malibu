#include "asset.hpp"
#include "fs.hpp"

using namespace core;

namespace asset {
	struct Manager {
		
	};

	static Option<Manager> g_manager;

	void init() {
		g_manager = Manager {};
	}
}