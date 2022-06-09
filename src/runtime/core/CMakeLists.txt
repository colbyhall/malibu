add_library (core STATIC
        fs.cpp
        containers/string.cpp
        memory.cpp
        time.cpp
        win32_fs.cpp
        win32_library.cpp
        win32_time.cpp
        win32_window.cpp
        math/math_utility.cpp
        math/color.cpp
        win32_thread.cpp
)

target_include_directories (core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})