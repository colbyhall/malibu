project "fbx"
    kind "StaticLib"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }

	includedirs { "../../runtime/*", "2020.2.1/include/" }

	cppdialect "C++20"

	links { "core", "2020.2.1/lib/libfbxsdk.lib" }

	filter { "system:windows" }
		postbuildcommands { "copy %{wks.location}..\\..\\src\\third_party\\fbx\\2020.2.1\\lib\\libfbxsdk.dll %{wks.location}..\\..\\bin\\%{cfg.buildcfg}\\libfbxsdk.dll" }