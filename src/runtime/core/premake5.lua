project "core"
	kind "StaticLib"
	language "C++"

	files { "**.hpp", "**.cpp", "**.lua", "**.natvis" }

	vpaths { ["*"] = { "**.hpp", "**.cpp", "**.lua", "**.natvis" } }

	cppdialect "C++17"

	flags { "FatalWarnings" }