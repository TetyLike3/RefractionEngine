cmake_minimum_required(VERSION 4.0)
project(Engine VERSION 1.0 LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare(
		assimp
		GIT_REPOSITORY https://github.com/assimp/assimp
		GIT_TAG v6.0.5
)

add_library(Engine STATIC)
set_target_properties(Engine PROPERTIES
		OUTPUT_NAME Refraction${PROJECT_NAME}
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

file(GLOB_RECURSE ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
target_sources(Engine
		PRIVATE
		lib/include/imgui/imgui_demo.cpp
		lib/include/imgui/imgui_draw.cpp
		lib/include/imgui/imgui_impl_glfw.cpp
		lib/include/imgui/imgui_impl_opengl3.cpp
		lib/include/imgui/imgui_tables.cpp
		lib/include/imgui/imgui_widgets.cpp
		lib/include/imgui/imgui.cpp
		src/Rendering/glad.c
		${ENGINE_SOURCES}
)

target_compile_features(Engine
	PRIVATE
		cxx_std_20
		c_std_17
)
target_compile_definitions(Engine
		PRIVATE
		_LIB
		UNICODE
		_UNICODE
		"$<$<CONFIG:Debug>:_DEBUG>"
		"$<$<CONFIG:Release>:NDEBUG>"
		"$<$<STREQUAL:${CMAKE_CXX_COMPILER_ARCHITECTURE_ID},X86>:WIN32>"
)

target_include_directories(Engine
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/lib/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

FetchContent_MakeAvailable(assimp)

target_link_libraries(Engine
		PRIVATE
		cpptrace::cpptrace
		glfw
		assimp
		zlibstatic
)

target_link_directories(Engine
		PRIVATE
		"$<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/Libs/x64_Debug>"
		"$<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/Libs/x64_Release>"
)

if(WIN32)
	target_compile_definitions(Engine PUBLIC RFCT_PLATFORM_WINDOWS)
endif()
