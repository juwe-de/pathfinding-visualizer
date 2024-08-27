#include "application.h"
#include "glad/glad.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <iostream>

int Application::run() {

    GLFWwindow* window;

    if(!glfwInit()) return -1;

    window = glfwCreateWindow(640, 400, "Pathfinding Visualizer", NULL, NULL);
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to load OpenGL." << std::endl;
        glfwTerminate();
        return -1;
    }

    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImDrawList* foregroundDrawList = ImGui::GetForegroundDrawList();
        ImDrawList* backgroundDrawList = ImGui::GetBackgroundDrawList();

        // ===============
        // GUI STARTS HERE
        // ===============
        
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        ImVec2 windowSize = ImVec2(width, height);
        ImVec2 windowCenter = ImVec2(windowSize.x * 0.5f, windowSize.y * 0.5f);
        ImVec2 gridUpperLeft = ImVec2(250, windowCenter.y - windowSize.y * 0.48f);
        ImVec2 gridBottomRight = ImVec2(windowCenter.x + windowSize.x * 0.48f, windowCenter.y + windowSize.y * 0.48f);
        ImVec2 gridDimensions = ImVec2(gridBottomRight.x - gridUpperLeft.x, gridBottomRight.y - gridUpperLeft.y);
        
        float tileSize = std::min(gridDimensions.x / GRID_COLS, gridDimensions.y / GRID_ROWS);
        ImVec2 tilePos = gridUpperLeft;

        for(auto i = 0; i < GRID_ROWS; i++) {

            for(auto j = 0; j < GRID_COLS; j++) {

                backgroundDrawList->AddRectFilled(tilePos, ImVec2(tilePos.x + tileSize, tilePos.y + tileSize), IM_COL32(255, 255, 255, 255));
                foregroundDrawList->AddRect(tilePos, ImVec2(tilePos.x + tileSize, tilePos.y + tileSize), IM_COL32(150, 150, 150, 255));
                tilePos.x += tileSize;

            }

            if(i < GRID_ROWS - 1) tilePos.x = gridUpperLeft.x;
            tilePos.y += tileSize;

        }

        gridBottomRight = tilePos;
        foregroundDrawList->AddRect(gridUpperLeft, gridBottomRight, IM_COL32(0, 0, 0, 255), 0, 0, 3.0f);
        
        // ===============
        // GUI ENDS HERE
        // ===============

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
    
}