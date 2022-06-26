project "gpu"
    kind "StaticLib"
    language "C++"

    files { "**.hpp", "**.cpp", "**.h" }

    vpaths { ["*"] = { "**.hpp", "**.cpp", "**.h" } }

    includedirs { "../*" }

	systemversion "latest"

    links { "core", "dxgi" }

	cppdialect "C++20"

	flags { "FatalWarnings" }