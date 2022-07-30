#pragma once

#include "templates/type_traits.hpp"
#include "uuid.hpp"
#include "core.hpp"

namespace asset {

	class AssetRegistration {
	public:
		AssetRegistration(StringView name);

	private:
		StringView m_name;
	};

#define REGISTER_ASSET(asset_name) \
	static asset::AssetRegistration _registration_##asset_name = { #asset_name };

	class Asset {
	public:
		virtual StringView class_name() const = 0;

	};

#define ASSET_DECLARATION(name) \
	StringView class_name() const override { return #name; }

	template <typename T>
	class AssetRef {
		static_assert(core::is_base_of<Asset, T>, "T must be child of abstract class asset::Asset");
	public:

	private:
		inline AssetRef(UUID resource_id) : m_resource_id(resource_id) {}
		UUID m_resource_id;
	};

	void init();
	void query();
}