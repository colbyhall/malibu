project "draw"
    kind "StaticLib"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }

    includedirs { "../*", "../../third_party/*" }

	systemversion "latest"

    links { "core", "stb", "gpu", "asset" }

	cppdialect "C++17"

	flags { "FatalWarnings" }