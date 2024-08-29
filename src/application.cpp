#include "application.h"
#include "glad/glad.h"
#include "graph.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <algorithm>
#include <cmath>
#include <GLFW/glfw3.h>
#include <memory>
#include <iostream>
#include <utility>

#define TILE_BORDER_COLOR IM_COL32(150, 150, 150, 150)
#define BLACK IM_COL32(0, 0, 0, 255)
#define WHITE IM_COL32(255, 255, 255, 255)

#define NODE_NULL Graph::Node(-1, -1, 0)

#define GRID_ROWS 40
#define GRID_COLS 60

template <typename T>
bool vectorHasElement(vector<T> vec, T el) {
    return std::find(vec.begin(), vec.end(), el) != vec.end();
}

std::pair<Graph::Node, Graph::Node> toBeConnected = std::make_pair<Graph::Node, Graph::Node>(NODE_NULL, NODE_NULL);

Graph::Node getNodeUnderMouse(const ImVec2 mousePos, const ImVec2 gridUpperLeft, const ImVec2 gridBottomRight, float tileSize) {

    if(mousePos.x < gridUpperLeft.x || mousePos.x > gridBottomRight.x || mousePos.y < gridUpperLeft.y || mousePos.y > gridBottomRight.y) return NODE_NULL;

    ImVec2 tileCoordinates = ImVec2(std::floor((mousePos.x - gridUpperLeft.x) / tileSize), std::floor((mousePos.y - gridUpperLeft.y) / tileSize));
    Graph::Node nodeUnderMouse = Graph::Node(tileCoordinates.x, tileCoordinates.y, GRID_COLS);

    return nodeUnderMouse;

}

void handleLeftMouseButton(const std::shared_ptr<Graph> graph, const ImVec2 gridUpperLeft, const ImVec2 gridBottomRight, const float tileSize) {

    if(!ImGui::IsMouseDown(ImGuiMouseButton_Left)) return;

    ImVec2 mousePos = ImGui::GetMousePos();

    Graph::Node nodeUnderMouse = getNodeUnderMouse(mousePos, gridUpperLeft, gridBottomRight, tileSize);
    if(nodeUnderMouse == NODE_NULL) return;

    if(toBeConnected.first == NODE_NULL) {
        toBeConnected.first = nodeUnderMouse;
        return;
    }  
    
    if(toBeConnected.first == nodeUnderMouse) return;

    toBeConnected.second = nodeUnderMouse;

    // tiles are diagonally aligned; not next to each other
    if(std::abs(toBeConnected.first.x - toBeConnected.second.x) + std::abs(toBeConnected.first.y - toBeConnected.second.y) > 1) {
        toBeConnected.first = toBeConnected.second;
        return;
    }

    graph->addEdge(toBeConnected.first, toBeConnected.second);
    toBeConnected.first = toBeConnected.second;

}

void handleRightMouseButton(const std::shared_ptr<Graph> graph, const ImVec2 gridUpperLeft, const ImVec2 gridBottomRight, const float tileSize) {

    if(!ImGui::IsMouseDown(ImGuiMouseButton_Right)) return;

    ImVec2 mousePos = ImGui::GetMousePos();

    Graph::Node nodeUnderMouse = getNodeUnderMouse(mousePos, gridUpperLeft, gridBottomRight, tileSize);
    if(nodeUnderMouse == NODE_NULL) return;

    graph->removeAllNeighbors(nodeUnderMouse);

}

int Application::run() {

    std::shared_ptr<Graph> graph = std::make_shared<Graph>(GRID_ROWS, GRID_COLS);

    // ---------------------------------------------

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

        for(auto row = 0; row < GRID_ROWS; row++) {

            for(auto col = 0; col < GRID_COLS; col++) {

                backgroundDrawList->AddRectFilled(tilePos, ImVec2(tilePos.x + tileSize, tilePos.y + tileSize), WHITE);
                foregroundDrawList->AddRect(tilePos, ImVec2(tilePos.x + tileSize, tilePos.y + tileSize), TILE_BORDER_COLOR, 0, 0, 0.2f);

                Graph::Node currentNode = Graph::Node(col, row, GRID_COLS);
                vector<Graph::Node> neighbourNodes = col >= 0 && row >= 0 ? graph->getNeighbors(currentNode) : vector<Graph::Node>();

                if(!vectorHasElement<Graph::Node>(neighbourNodes, Graph::Node(col - 1, row, GRID_COLS))) {
                    foregroundDrawList->AddLine(tilePos, ImVec2(tilePos.x, tilePos.y + tileSize), BLACK, 3.0f);
                }
                if(!vectorHasElement<Graph::Node>(neighbourNodes, Graph::Node(col, row - 1, GRID_COLS))) {
                    foregroundDrawList->AddLine(tilePos, ImVec2(tilePos.x + tileSize, tilePos.y), BLACK, 3.0f);
                }
                if(!vectorHasElement<Graph::Node>(neighbourNodes, Graph::Node(col + 1, row, GRID_COLS))) {
                    foregroundDrawList->AddLine(ImVec2(tilePos.x + tileSize, tilePos.y), ImVec2(tilePos.x + tileSize, tilePos.y + tileSize), BLACK, 3.0f);
                }
                if(!vectorHasElement<Graph::Node>(neighbourNodes, Graph::Node(col, row + 1, GRID_COLS))) {
                    foregroundDrawList->AddLine(ImVec2(tilePos.x, tilePos.y + tileSize), ImVec2(tilePos.x + tileSize, tilePos.y + tileSize), BLACK, 3.0f);
                }

                tilePos.x += tileSize;
                
            }

            if(row < GRID_ROWS - 1) tilePos.x = gridUpperLeft.x;
            tilePos.y += tileSize;

        }

        gridBottomRight = tilePos;
        foregroundDrawList->AddRect(gridUpperLeft, gridBottomRight, BLACK, 0, 0, 3.0f);

        handleLeftMouseButton(graph, gridUpperLeft, gridBottomRight, tileSize);
        handleRightMouseButton(graph, gridUpperLeft, gridBottomRight, tileSize);
        
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