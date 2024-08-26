cmake_minimum_required(VERSION 3.22.1)

project(pathfinding-visualizer)

set(SOURCES
    src/main.cpp
    src/glad.c
    src/application.cpp
)
set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

add_subdirectory(external/glfw)

target_include_directories(${PROJECT_NAME}
    PUBLIC external/glfw/include
    PUBLIC external/imgui
)

target_link_directories(${PROJECT_NAME}
    PRIVATE external/glfw/src
)

target_link_libraries(${PROJECT_NAME}
    glfw
)