workspace "malibu"
	architecture "x64"
    
	configurations { "Debug", "Release" }

	targetdir ("bin/%{cfg.buildcfg}")
	objdir ("bin/%{cfg.buildcfg}")
	debugdir ("bin/%{cfg.buildcfg}")
	location ("build/" .. _ACTION)

	characterset("ASCII")

	filter "configurations:Debug"
    	defines { "BUILD_DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "BUILD_RELEASE" }
		optimize "On"

	include "src/runtime"
	include "src/programs"