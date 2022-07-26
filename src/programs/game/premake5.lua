project "game"
	kind "WindowedApp"
	language "C++"

	files { "**.hpp", "**.cpp" }

	vpaths { ["*"] = { "**.hpp", "**.cpp" } }
	includedirs { "../../runtime/*", "../../third_party/*" }

	links { "core", "gpu", "fbx", "asset" }

	cppdialect "C++17"

	flags { "FatalWarnings" }

	debugdir "../../../"