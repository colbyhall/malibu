#pragma once

#include "templates/type_traits.hpp"
#include "uuid.hpp"

namespace asset {
	class Asset {
	public:
		
	};

	template <typename T>
	class AssetRef {
		static_assert(core::is_base_of<Resource, T>, "T must be child of abstract class asset::Asset");
	public:



	private:
		inline AssetRef(UUID resource_id) : m_resource_id(resource_id) {}
		UUID m_resource_id;
	};

	void init();
}