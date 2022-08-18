project "stb"
    kind "StaticLib"
    language "C++"

    files { "**.h", "**.cpp" }

    vpaths { ["*"] = { "**.h", "**.cpp" } }

	includedirs { "../../runtime/*" }

	cppdialect "C++17"

	links { "core" }