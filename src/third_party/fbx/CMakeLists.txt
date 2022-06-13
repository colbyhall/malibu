add_library (fbx STATIC 
	fbx.cpp
)

target_link_libraries (fbx core)
target_link_libraries(fbx "${CMAKE_CURRENT_SOURCE_DIR}/2020.2.1/lib/libfbxsdk.lib")

target_include_directories (fbx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (fbx PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/2020.2.1/include/")