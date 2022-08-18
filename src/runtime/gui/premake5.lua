project "gui"
    kind "StaticLib"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }

    includedirs { "../*", "../../third_party/*" }

	systemversion "latest"

    links { "core", "gpu", "draw", "dxc" }

	cppdialect "C++17"

	flags { "FatalWarnings" }