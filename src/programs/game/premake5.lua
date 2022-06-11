project "game"
	kind "WindowedApp"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }
    includedirs { "../../runtime" }

    links { "core", "gpu" }

    cppdialect "C++20"

	flags { "FatalWarnings" }

    debugdir "../../"
