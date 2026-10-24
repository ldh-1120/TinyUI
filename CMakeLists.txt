cmake_minimum_required(VERSION 3.28)

project(TinyUI
        VERSION 0.1.0
        LANGUAGES CXX
)

include(CTest)
include(cmake/CompilerOptions.cmake)

option(TINYUI_BUILD_EXAMPLES "Build TinyUI examples" ON)
option(TINYUI_BUILD_VULKAN "Build the Vulkan backend" ON)

add_subdirectory(src/Foundation)
add_subdirectory(src/Platform)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(TINYUI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(TINYUI_BUILD_VULKAN)
    add_subdirectory(src/Render/Vulkan)
endif()