find_package(Vulkan REQUIRED)

add_library(tinyui_vulkan STATIC
        VulkanRuntime.cpp
)

add_library(TinyUI::Vulkan
        ALIAS tinyui_vulkan
)

target_include_directories(tinyui_vulkan PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(tinyui_vulkan
        PUBLIC TinyUI::Foundation
        PRIVATE Vulkan::Vulkan
)

tinyui_set_warnings(tinyui_vulkan)
