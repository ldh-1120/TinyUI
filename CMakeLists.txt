cmake_minimum_required(VERSION 4.3)
project(TinyUI VERSION 0.1.0 LANGUAGES CXX)

add_library(tinyui_compiler_options INTERFACE)

if(MSVC)
    target_compile_options(tinyui_compiler_options INTERFACE /std:c++latest /Zc:__cplusplus)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(tinyui_compiler_options INTERFACE -std=c++2c)
else()
    message(FATAL_ERROR "Unsupported C++ compiler")
endif()

include(CTest)
include(cmake/CompilerOptions.cmake)

option(TINYUI_BUILD_EXAMPLES "Build TinyUI examples" ON)
add_subdirectory(src/Foundation)
add_subdirectory(src/Platform)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()

if (TINYUI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()