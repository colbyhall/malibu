project "core"
    kind "StaticLib"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }

	cppdialect "C++20"

	flags { "FatalWarnings" }