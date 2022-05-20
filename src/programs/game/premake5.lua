project "game"
    kind "ConsoleApp"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }
    includedirs { "../../runtime" }

    links { "core", "gpu", "user32", "kernel32", "shlwapi" }

    cppdialect "C++20"
