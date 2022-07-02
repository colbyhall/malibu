project "game"
	kind "WindowedApp"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }
    includedirs { "../../runtime/*", "../../third_party/*" }

    links { "core", "gpu", "fbx" }

    cppdialect "C++20"

	flags { "FatalWarnings" }

    debugdir "../../../"