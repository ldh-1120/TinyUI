add_library(tinyui_compiler_options INTERFACE
        ../include/TinyUI/Render/VulkanRuntime.h
        ../include/TinyUI/Core/NativeWindow.h)

if(MSVC)
    target_compile_options(tinyui_compiler_options INTERFACE
            /std:c++latest
            /Zc:__cplusplus
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(tinyui_compiler_options INTERFACE
            -std=c++2c
    )
else()
    message(FATAL_ERROR "Unsupported C++ compiler")
endif()

function(tinyui_set_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
                /W4
                /permissive-
        )
    else()
        target_compile_options(${target} PRIVATE
                -Wall
                -Wextra
                -Wpedantic
        )
    endif()
endfunction()
