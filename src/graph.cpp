#include "graph.h"

#include <algorithm>

Graph::Node::Node(const int gridX, const int gridY, const int cols): id(gridY * cols + gridX), x(gridX), y(gridY) {}

Graph::Graph(int rows, int cols) {

    m_adjList = std::make_unique<map<Node, vector<Node>>>();

    for(auto x = 0; x < cols; x++) {
        for(auto y = 0; y < rows; y++) {

            Node node = Node(x, y, cols);
            m_adjList->insert(std::make_pair(node, vector<Node>()));

        }
    }

}

void Graph::addEdge(const Node a, const Node b) {

    m_adjList->at(a).push_back(b);
    m_adjList->at(b).push_back(a);
    
}

void Graph::removeAllNeighbors(const Node node) {
    
    vector<Node> neighbors = m_adjList->at(node);

    if(neighbors.empty()) return;

    for(Node &neihgbor : neighbors) {

        auto nodePos = std::find(m_adjList->at(neihgbor).begin(), m_adjList->at(neihgbor).end(), node);
        if(nodePos != m_adjList->at(neihgbor).end()) m_adjList->at(neihgbor).erase(nodePos);

    }

    m_adjList->at(node) = vector<Node>();

}

vector<Graph::Node> Graph::getNeighbors(const Node node) const {
    return m_adjList->at(node);
}