cmake_minimum_required(VERSION 4.0)
project(Runtime LANGUAGES CXX)

include(FetchContent)
# switch to luabridge3!!!!!
FetchContent_Declare(
        sol2
        GIT_REPOSITORY https://github.com/ThePhD/sol2
        GIT_TAG        v3.3.0
)

add_library(Runtime SHARED)

target_sources(Runtime
    PRIVATE
        src/Lua.cpp
        src/Runtime.cpp
)

set_target_properties(Runtime PROPERTIES OUTPUT_NAME Refraction${PROJECT_NAME})

target_compile_features(Runtime
    PRIVATE
        cxx_std_20
        c_std_17
)

target_include_directories(Runtime
    PRIVATE
        ${CMAKE_SOURCE_DIR}/Engine/src
        ${CMAKE_SOURCE_DIR}/Engine/lib/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(Runtime
    PRIVATE
        _CONSOLE
        UNICODE
        _UNICODE
        "$<$<CONFIG:Debug>:_DEBUG>"
        "$<$<CONFIG:Release>:NDEBUG>"
        "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ARCHITECTURE_ID},X86>:WIN32>"
)

target_link_directories(Runtime
    PRIVATE
        "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ARCHITECTURE_ID},x64>:$<TARGET_FILE_DIR:Runtime>>"
        "$<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/Libs/x64_Debug>"
        "$<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/Libs/x64_Release>"
)

FetchContent_MakeAvailable(sol2)

target_link_libraries(Runtime
    PUBLIC
        Engine
    PRIVATE
        sol2::sol2
        glfw3dll
)
