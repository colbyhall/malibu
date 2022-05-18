project "hello_world"
    kind "ConsoleApp"
    language "C++"

    files { "**.hpp", "**.cpp" }

    vpaths { ["*"] = { "**.hpp", "**.cpp" } }
    dependson "core"
    includedirs { "../../runtime" }
