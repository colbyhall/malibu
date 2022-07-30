project "gui"
    kind "StaticLib"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }

    includedirs { "../*" }

	systemversion "latest"

    links { "core", "gpu", "draw2d" }

	cppdialect "C++17"

	flags { "FatalWarnings" }