project "gpu"
    kind "StaticLib"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }

    includedirs { "../" }

    links { "core", "d3d12", "dxgi", "d3dcompiler" }

	cppdialect "C++20"