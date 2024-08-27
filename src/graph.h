#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <memory>
#include <vector>

using std::map, std::vector;

class Graph {

    struct Node {
        int x;
        int y;

        Node(int gridX, int gridY);

        friend bool operator<(const Node &a, const Node &b) { return a.x < b.x && a.y < b.y; }
    };

public:
    Graph(int rows, int cols);
    void addEdge(const Node a, const Node b); 
    vector<Node> getNeighbours(const Node node) const;

private:
    std::unique_ptr<map<Node, vector<Node>>> m_adjList;

};

#endif