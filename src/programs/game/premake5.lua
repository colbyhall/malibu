project "game"
    kind "ConsoleApp"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }
    includedirs { "../../runtime" }

    links { "core" }
