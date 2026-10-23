add_executable(TinyUIMinimal Minimal/main.cpp)
target_link_libraries(TinyUIMinimal PRIVATE TinyUI::Foundation)

tinyui_set_warnings(TinyUIMinimal)

add_executable(TinyUIWindow Window/main.cpp)
target_link_libraries(TinyUIWindow PRIVATE TinyUI::Core)

tinyui_set_warnings(TinyUIWindow)