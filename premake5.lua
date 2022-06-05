workspace "malibu"
	architecture "x64"
    
	configurations { "debug", "release" }

	targetdir ("bin/%{cfg.buildcfg}")
	objdir ("bin/%{cfg.buildcfg}")
	debugdir ("bin/%{cfg.buildcfg}")
	location ("build/" .. _ACTION)

	characterset("ASCII")

	filter "configurations:debug"
    	defines { "BUILD_DEBUG#1" }
		symbols "On"

	filter "configurations:release"
		defines { "BUILD_RELEASE#1" }
		optimize "On"

	include "src/runtime"
	include "src/programs"

	flags { "FatalWarnings" }
