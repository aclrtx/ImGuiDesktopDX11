cmake_minimum_required(VERSION 4.3)
project(ImGuiDesktopDX11)

add_subdirectory(Vendor)

add_executable(${PROJECT_NAME} WIN32 Source/Main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE Include)
target_link_libraries(${PROJECT_NAME} PRIVATE ImGui d3d11)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)
