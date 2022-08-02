project "dxc"
    kind "StaticLib"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }

	includedirs { "../../runtime/*", "include" }

	cppdialect "C++17"

	links { "core", "gpu", "bin/dxcompiler.lib" }

	filter { "system:windows" }
		postbuildcommands { 
			"copy %{wks.location}..\\..\\src\\third_party\\dxc\\bin\\dxcompiler.dll %{wks.location}..\\..\\bin\\%{cfg.buildcfg}\\dxcompiler.dll",
			"copy %{wks.location}..\\..\\src\\third_party\\dxc\\bin\\dxil.dll %{wks.location}..\\..\\bin\\%{cfg.buildcfg}\\dxil.dll"
		}