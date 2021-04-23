#ifndef KRUSKAL_HPP
#define KRUSKAL_HPP

#include "Nodes.hpp"
#include <algorithm>

class KruskalMST
{
public:
    KruskalMST(const std::vector<Node2D> &inputNodes);
    void genMST();
    bool stepMST();

    const std::vector<Node2D> &nodes;
    std::vector<Edge> edges;
    std::vector<Edge> chosenEdges;

    std::vector<ID> edgeIds;
    std::vector<bool> visited;

    ID curEdgeId;
private:
    ID root(ID x);
    void join(ID x, ID y);

    bool isDone();
    void createEdges();
    unsigned int numVisited;
};

#endif //KRUSKAL_HPP