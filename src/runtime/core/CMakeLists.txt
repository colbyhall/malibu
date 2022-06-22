add_library (core STATIC
		async/atomic.hpp
        async/mutex.hpp

		containers/array.hpp
		containers/bitflag.hpp
		containers/function.hpp
		containers/non_null.hpp
		containers/option.hpp
		containers/result.hpp
		containers/shared_ref.hpp
		containers/slice.hpp
		containers/string.hpp
		containers/string.cpp
		containers/unique.hpp

		math.hpp
		math/aabb2.hpp
		math/aabb3.hpp
		math/color.hpp
		math/color.cpp
		math/mat3.hpp
		math/mat4.hpp
		math/math_utility.hpp
		math/math_utility.cpp
		math/quat.hpp
		math/vec2.hpp
		math/vec3.hpp
		math/vec4.hpp

		core.hpp

		fiber.hpp
		win32_fiber.cpp

		fs.hpp
		fs.cpp
		win32_fs.cpp

		library.hpp
		win32_library.cpp

		memory.hpp
		memory.cpp

		primitives.hpp

		thread.hpp
		win32_thread.cpp

		time.hpp
		time.cpp
		win32_time.cpp

		type_traits.hpp

		window.hpp
		win32_window.cpp
)

target_include_directories (core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})