project(ImGui)

add_library(${PROJECT_NAME} STATIC
    Source/imgui.cpp
    Source/imgui_demo.cpp
    Source/imgui_draw.cpp
    Source/imgui_impl_dx11.cpp
    Source/imgui_impl_win32.cpp
    Source/imgui_tables.cpp
    Source/imgui_widgets.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC Include)
