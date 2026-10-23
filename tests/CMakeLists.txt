find_package(Catch2 3 CONFIG REQUIRED)

include(Catch)

add_executable(tinyui_tests
        FoundationTests.cpp
        PlatformTests.cpp
)

target_link_libraries(tinyui_tests PRIVATE
        TinyUI::Foundation
        Catch2::Catch2WithMain
)

if(WIN32)
    target_sources(tinyui_tests PRIVATE
            PlatformTests.cpp
    )

    target_link_libraries(tinyui_tests PRIVATE
            TinyUI::Core
    )
endif()

tinyui_set_warnings(tinyui_tests)

catch_discover_tests(tinyui_tests)
