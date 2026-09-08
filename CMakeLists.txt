cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Force minimum policy version")

project(Refraction)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.5.1
)

FetchContent_Declare(
        cpptrace
        GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace
        GIT_TAG v1.0.4
)

FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(cpptrace)

add_subdirectory(Engine)
#add_subdirectory(Runtime)
add_subdirectory(Editor)
