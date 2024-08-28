#include "graph.h"

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

vector<Graph::Node> Graph::getNeighbours(const Node node) const {
    return m_adjList->at(node);
}