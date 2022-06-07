add_library (gpu resources.cpp pipelines.cpp context.cpp command_list.cpp dx12/dx12_resources.cpp dx12/dx12_pipelines.cpp dx12/dx12_context.cpp dx12/dx12_command_list.cpp)
target_link_libraries (gpu core)
target_link_libraries(gpu "dxgi.lib")
target_include_directories (gpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})