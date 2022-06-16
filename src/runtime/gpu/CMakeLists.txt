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

		d3d12/d3d12_utility.hpp

		d3d12/d3d12_resources.hpp
		d3d12/d3d12_resources.cpp

		d3d12/d3d12_pipelines.hpp
		d3d12/d3d12_pipelines.cpp

		d3d12/d3d12_context.hpp
		d3d12/d3d12_context.cpp

		d3d12/d3d12_command_list.hpp
		d3d12/d3d12_command_list.cpp
)

target_link_libraries (gpu core)
target_link_libraries(gpu "dxgi.lib")
target_include_directories (gpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})