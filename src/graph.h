#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <memory>
#include <vector>

using std::map, std::vector;

class Graph {

public:
    struct Node {
        int id;
        int x;
        int y;

        Node(const int gridX, const int gridY, const int cols);

        friend bool operator<(const Node &a, const Node &b) { return a.id < b.id; }
        friend bool operator==(const Node &a, const Node &b) { return a.id == b.id; }
        friend bool operator!=(const Node &a, const Node &b) { return !operator==(a, b); }
    };

    Graph(const int rows, const int cols);
    void resize(const int rows, const int cols);
    void addEdge(const Node a, const Node b);
    void removeAllNeighbors(const Node node); 

    vector<Node> getNeighbors(const Node node) const;

private:
    std::unique_ptr<map<Node, vector<Node>>> m_adjList;

};

#endif