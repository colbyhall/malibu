project "gpu"
    kind "StaticLib"
    language "C++"

    files { "**.hpp", "**.cpp", "**.h" }

    vpaths { ["*"] = { "**.hpp", "**.cpp", "**.h" } }

    includedirs { "../" }

	systemversion "latest"

    links { "core", "d3d12", "dxgi", "d3dcompiler" }

	cppdialect "C++20"