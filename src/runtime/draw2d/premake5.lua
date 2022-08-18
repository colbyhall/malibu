project "draw2d"
    kind "StaticLib"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }

    includedirs { "../*", "../../third_party/*" }

	systemversion "latest"

    links { "core", "stb" }

	cppdialect "C++17"

	flags { "FatalWarnings" }