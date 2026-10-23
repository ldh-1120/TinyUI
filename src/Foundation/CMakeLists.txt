add_library(tinyui_foundation INTERFACE ../../include/TinyUI/Foundation/Slotmap.h)
add_library(TinyUI::Foundation ALIAS tinyui_foundation)

target_link_libraries(tinyui_foundation INTERFACE tinyui_compiler_options)
target_include_directories(tinyui_foundation INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_sources(tinyui_foundation INTERFACE
        FILE_SET HEADERS
        BASE_DIRS "${PROJECT_SOURCE_DIR}/include"
        FILES
        "${PROJECT_SOURCE_DIR}/include/TinyUI/Foundation/Geometry.h"
        "${PROJECT_SOURCE_DIR}/include/TinyUI/Foundation/Handle.h"
        "${PROJECT_SOURCE_DIR}/include/TinyUI/Foundation/Result.h"
        "${PROJECT_SOURCE_DIR}/include/TinyUI/Foundation/SlotMap.h"
)