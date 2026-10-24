if (WIN32)
    add_library(tinyui_core STATIC Win32/Win32Platform.cpp
            ../Render/Vulkan/VulkanRuntime.cpp)
else ()
    message(FATAL_ERROR "Native backend is not implemented for this platform yet.")
endif ()

add_library(TinyUI::Core ALIAS tinyui_core)
target_include_directories(tinyui_core PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(tinyui_core PUBLIC TinyUI::Foundation)
target_link_libraries(tinyui_core PRIVATE user32)

tinyui_set_warnings(tinyui_core)
