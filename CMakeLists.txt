cmake_minimum_required(VERSION 3.22.1)

project(pathfinding-visualizer)

add_executable(${PROJECT_NAME} src/main.cpp src/glad.c)

add_subdirectory(external/glfw)

target_include_directories(${PROJECT_NAME}
    PUBLIC external/glfw/include
)

target_link_directories(${PROJECT_NAME}
    PRIVATE external/glfw/src
)

target_link_libraries(${PROJECT_NAME}
    glfw
)