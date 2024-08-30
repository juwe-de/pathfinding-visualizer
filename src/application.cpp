#include "application.h"
#include "glad/glad.h"
#include "graph.h"

#define IM_VEC2_CLASS_EXTRA friend bool operator==(const ImVec2 &a, const ImVec2 &b) {return a.x == b.x && a.y == b.y; }

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

template <typename T>
bool vectorHasElement(vector<T> vec, T el) {
    return std::find(vec.begin(), vec.end(), el) != vec.end();
}

std::pair<Graph::Node, Graph::Node> toBeConnected = std::make_pair<Graph::Node, Graph::Node>(NODE_NULL, NODE_NULL);

Graph::Node getNodeUnderMouse(const ImVec2 mousePos, const ImVec2 gridUpperLeft, const ImVec2 gridBottomRight, float tileSize, int *cols) {

    if(mousePos.x < gridUpperLeft.x || mousePos.x > gridBottomRight.x || mousePos.y < gridUpperLeft.y || mousePos.y > gridBottomRight.y) return NODE_NULL;

    ImVec2 tileCoordinates = ImVec2(std::floor((mousePos.x - gridUpperLeft.x) / tileSize), std::floor((mousePos.y - gridUpperLeft.y) / tileSize));
    Graph::Node nodeUnderMouse = Graph::Node(tileCoordinates.x, tileCoordinates.y, *cols);

    return nodeUnderMouse;

}

void handleLeftMouseButton(const std::shared_ptr<Graph> graph, const ImVec2 gridUpperLeft, const ImVec2 gridBottomRight, const float tileSize, int *cols) {

    if(!ImGui::IsMouseDown(ImGuiMouseButton_Left)) return;

    ImVec2 mousePos = ImGui::GetMousePos();

    Graph::Node nodeUnderMouse = getNodeUnderMouse(mousePos, gridUpperLeft, gridBottomRight, tileSize, cols);
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

void handleRightMouseButton(const std::shared_ptr<Graph> graph, const ImVec2 gridUpperLeft, const ImVec2 gridBottomRight, const float tileSize, int *cols) {

    if(!ImGui::IsMouseDown(ImGuiMouseButton_Right)) return;

    ImVec2 mousePos = ImGui::GetMousePos();

    Graph::Node nodeUnderMouse = getNodeUnderMouse(mousePos, gridUpperLeft, gridBottomRight, tileSize, cols);
    if(nodeUnderMouse == NODE_NULL) return;

    graph->removeAllNeighbors(nodeUnderMouse);

}

void showControlsWindow(const std::shared_ptr<Graph> graph, int *rows, int *cols, ImVec2 *startPos, ImVec2 *targetPos) {

    ImGui::Begin("Controls");

    if(ImGui::CollapsingHeader("Maze Generation")) {

        const char *mazeGenerationAlgorithms[] = { "Select Algorithm", "Kruskals" };
        static const char *currentMaze = mazeGenerationAlgorithms[0];

        if(ImGui::BeginCombo("##combo", currentMaze)) {

            for(int i = 0; i < IM_ARRAYSIZE(mazeGenerationAlgorithms); i++) {

                bool isSelected = currentMaze == mazeGenerationAlgorithms[i];

                if(ImGui::Selectable(mazeGenerationAlgorithms[i], isSelected)) {
                    currentMaze = mazeGenerationAlgorithms[i];
                }

                if(isSelected) ImGui::SetItemDefaultFocus();

            }

            ImGui::EndCombo();

        }

        ImGui::Button("Generate Maze");

    }

    if(ImGui::CollapsingHeader("Pathfinding")) {

        const char *pathfindingAlgorithms[] = { "Select Algorithm", "Dijkstras" };
        static const char *currentPathfindingAlgo = pathfindingAlgorithms[0];

        if(ImGui::BeginCombo("##combo", currentPathfindingAlgo)) {

            for(int i = 0; i < IM_ARRAYSIZE(pathfindingAlgorithms); i++) {

                bool isSelected = currentPathfindingAlgo == pathfindingAlgorithms[i];

                if(ImGui::Selectable(pathfindingAlgorithms[i], isSelected)) {
                    currentPathfindingAlgo = pathfindingAlgorithms[i];
                }

                if(isSelected) ImGui::SetItemDefaultFocus();

            }

            ImGui::EndCombo();

        }

        ImGui::Button("Run Algorithm");

    }

    if(ImGui::CollapsingHeader("Grid")) {

        if(ImGui::SliderInt("Rows", rows, 1, 50)) {

            graph->resize(*rows, *cols);
            if(startPos->y >= *rows) startPos->y = *rows - 1;
            if(targetPos->y >= *rows) targetPos->y = *rows - 1; 

        }
        
        if(ImGui::SliderInt("Columns", cols, 1, 80)) {

            graph->resize(*rows, *cols);
            if(startPos->x >= *cols) startPos->x = *cols - 1;
            if(targetPos->x >= *cols) targetPos->x = *cols - 1;

        }

        

    }

    ImGui::End();

}

int Application::run() {

    int rows = 40;
    int cols = 60;

    std::shared_ptr<Graph> graph = std::make_shared<Graph>(rows, cols);
    ImVec2 startPos = ImVec2(0, 0);
    ImVec2 targetPos = ImVec2(cols - 1, rows - 1);

    constexpr ImColor startColor = IM_COL32(0, 255, 0, 255);
    constexpr ImColor targetColor = IM_COL32(255, 0, 0, 255);

    // ---------------------------------------------

    GLFWwindow* window;

    if(!glfwInit()) return -1;

    window = glfwCreateWindow(640, 400, "Pathfinding Visualizer", NULL, NULL);
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // ImFont *font = ImGui::GetFont();
    // font->Scale *= 2;
    // ImGui::PushFont(font);

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

        ImDrawList *foregroundDrawList = ImGui::GetForegroundDrawList();
        ImDrawList *backgroundDrawList = ImGui::GetBackgroundDrawList();

        // ===============
        // GUI STARTS HERE
        // ===============

        showControlsWindow(graph, &rows, &cols, &startPos, &targetPos);
        
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        ImVec2 windowSize = ImVec2(width, height);
        ImVec2 windowCenter = ImVec2(windowSize.x * 0.5f, windowSize.y * 0.5f);
        ImVec2 gridUpperLeft = ImVec2(250, windowCenter.y - windowSize.y * 0.48f);
        ImVec2 gridBottomRight = ImVec2(windowCenter.x + windowSize.x * 0.48f, windowCenter.y + windowSize.y * 0.48f);
        ImVec2 gridDimensions = ImVec2(gridBottomRight.x - gridUpperLeft.x, gridBottomRight.y - gridUpperLeft.y);
        
        float tileSize = std::min(gridDimensions.x / cols, gridDimensions.y / rows);
        ImVec2 tilePos = gridUpperLeft;

        for(auto row = 0; row < rows; row++) {

            for(auto col = 0; col < cols; col++) {
                
                ImColor tileColor;
                if(ImVec2(col, row) == startPos) tileColor = startColor;
                else if(ImVec2(col, row) == targetPos) tileColor = targetColor;
                else tileColor = WHITE;

                backgroundDrawList->AddRectFilled(tilePos, ImVec2(tilePos.x + tileSize, tilePos.y + tileSize), tileColor);
                foregroundDrawList->AddRect(tilePos, ImVec2(tilePos.x + tileSize, tilePos.y + tileSize), TILE_BORDER_COLOR, 0, 0, 0.2f);

                Graph::Node currentNode = Graph::Node(col, row, cols);
                vector<Graph::Node> neighbourNodes = col >= 0 && row >= 0 ? graph->getNeighbors(currentNode) : vector<Graph::Node>();

                if(!vectorHasElement<Graph::Node>(neighbourNodes, Graph::Node(col - 1, row, cols))) {
                    foregroundDrawList->AddLine(tilePos, ImVec2(tilePos.x, tilePos.y + tileSize), BLACK, 3.0f);
                }
                if(!vectorHasElement<Graph::Node>(neighbourNodes, Graph::Node(col, row - 1, cols))) {
                    foregroundDrawList->AddLine(tilePos, ImVec2(tilePos.x + tileSize, tilePos.y), BLACK, 3.0f);
                }
                if(!vectorHasElement<Graph::Node>(neighbourNodes, Graph::Node(col + 1, row, cols))) {
                    foregroundDrawList->AddLine(ImVec2(tilePos.x + tileSize, tilePos.y), ImVec2(tilePos.x + tileSize, tilePos.y + tileSize), BLACK, 3.0f);
                }
                if(!vectorHasElement<Graph::Node>(neighbourNodes, Graph::Node(col, row + 1, cols))) {
                    foregroundDrawList->AddLine(ImVec2(tilePos.x, tilePos.y + tileSize), ImVec2(tilePos.x + tileSize, tilePos.y + tileSize), BLACK, 3.0f);
                }

                tilePos.x += tileSize;
                
            }

            if(row < rows - 1) tilePos.x = gridUpperLeft.x;
            tilePos.y += tileSize;

        }

        gridBottomRight = tilePos;
        foregroundDrawList->AddRect(gridUpperLeft, gridBottomRight, BLACK, 0, 0, 3.0f);

        handleLeftMouseButton(graph, gridUpperLeft, gridBottomRight, tileSize, &cols);
        handleRightMouseButton(graph, gridUpperLeft, gridBottomRight, tileSize, &cols);
        
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