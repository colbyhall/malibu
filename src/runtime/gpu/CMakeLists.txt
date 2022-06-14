add_library (gpu STATIC
	gpu.hpp

	resources.hpp
	resources.cpp

	pipelines.hpp
	pipelines.cpp

	context.hpp
	context.cpp

	command_list.hpp
	command_list.cpp

	dx12/dx12_utility.hpp

	dx12/dx12_resources.hpp
	dx12/dx12_resources.cpp

	dx12/dx12_pipelines.hpp
	dx12/dx12_pipelines.cpp

	dx12/dx12_context.hpp
	dx12/dx12_context.cpp

	dx12/dx12_command_list.hpp
	dx12/dx12_command_list.cpp
)

target_link_libraries (gpu core)
target_link_libraries(gpu "dxgi.lib")
target_include_directories (gpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})