project "draw2d"
    kind "StaticLib"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }

    includedirs { "../*" }

	systemversion "latest"

    links { "core" }

	cppdialect "C++17"

	flags { "FatalWarnings" }