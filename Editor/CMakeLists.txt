cmake_minimum_required(VERSION 4.0)
project(Editor VERSION 1.0 LANGUAGES CXX)

add_executable(Editor)

file(GLOB_RECURSE EDITOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
target_sources(Editor PRIVATE ${EDITOR_SOURCES})

# Copy DLLs
file(GLOB ALL_DLLS ${CMAKE_SOURCE_DIR}/Libs/x64_Debug/*.dll)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ALL_DLLS} "${CMAKE_BINARY_DIR}/${PROJECT_NAME}"
)

target_compile_features(Editor
    PRIVATE
        cxx_std_20
        c_std_17
)

target_include_directories(Editor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/Engine/src
        ${CMAKE_SOURCE_DIR}/Engine/lib/include
        #${CMAKE_SOURCE_DIR}/Runtime/src
)

target_compile_definitions(Editor
        PRIVATE
        _CONSOLE
        UNICODE
        _UNICODE
        "$<$<CONFIG:Debug>:_DEBUG>"
        "$<$<CONFIG:Release>:NDEBUG>"
        "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ARCHITECTURE_ID},X86>:WIN32>"
)

target_link_directories(Editor
        PRIVATE
        "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ARCHITECTURE_ID},x64>:$<TARGET_FILE_DIR:Editor>>"
        "$<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/Libs/x64_Debug>"
        "$<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/Libs/x64_Release>"
)

target_link_libraries(Editor
        PRIVATE
        Engine
        cpptrace::cpptrace
        #Runtime
)

if(MINGW)
    target_link_options(Editor PRIVATE -Wl,--allow-shlib-undefined)
    target_link_libraries(Editor PRIVATE mingwex ucrt)
endif()