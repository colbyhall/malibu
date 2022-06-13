cmake_minimum_required (VERSION 3.8)
project (Malibu)

set(CMAKE_CXX_STANDARD 20)

set_property(GLOBAL PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

add_subdirectory (src)