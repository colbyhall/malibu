workspace "malibu"
	architecture "x64"
    
	configurations { "debug", "release" }

	targetdir ("bin/%{cfg.buildcfg}")
	objdir ("bin/%{cfg.buildcfg}")
	debugdir ("bin/%{cfg.buildcfg}")
	location ("build/" .. _ACTION)

	characterset("ASCII")

	filter "configurations:debug"
    	defines { "BUILD_DEBUG" }
		symbols "On"

	filter "configurations:release"
		defines { "BUILD_RELEASE" }
		optimize "On"

	include "src/runtime"
	include "src/programs"

	cppdialect "C++17"

	flags { "FatalWarnings" }