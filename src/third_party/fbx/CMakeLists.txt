add_library (fbx STATIC
	fbx.hpp
	fbx.cpp
)

target_link_libraries (fbx core "${CMAKE_CURRENT_SOURCE_DIR}/2020.2.1/lib/libfbxsdk.lib")

add_custom_command(TARGET fbx POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/2020.2.1/lib/"
		"${PROJECT_SOURCE_DIR}/build")

target_include_directories (fbx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (fbx PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/2020.2.1/include/")