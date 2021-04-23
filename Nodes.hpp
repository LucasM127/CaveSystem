#ifndef NODES_HPP
#define NODES_HPP

#include "SFVectorFuncs.hpp"
#include <vector>

typedef unsigned int ID;
typedef ID EDGEID;
typedef ID NODEID;
const ID INVALID_ID = -1;
   
inline ID next(ID i, ID sz)
{
    return i == sz - 1 ? 0 : i + 1;
}

inline ID last(ID i, ID sz)
{
    return i == 0 ? sz - 1 : i - 1;
}

//ie a circle
struct Node2D
{
    Node2D(Point p, float f) : pos(p), r(f) {}
    Point pos;
    float r;
};

//graph edge for nodes
struct Edge
{
    Edge(NODEID n1, NODEID n2, const std::vector<Node2D> &nodes)
            : a(n1 < n2 ? n1 : n2), b(n1 < n2 ? n2 : n1),
              w(SFUTIL::length<float>(nodes[a].pos-nodes[b].pos)) {}
    NODEID a;
    NODEID b;
    float w;//length
};

/*
inline std::vector<GraphNode> genGraph(const std::vector<Node2D> &nodes, const std::vector<Edge> &edges)
{
    std::vector<GraphNode> graph(nodes.size());
    for(auto &e : edges)
    {
        graph[e.a].neighbours.emplace_back(e.b);
        graph[e.b].neighbours.emplace_back(e.a);
    }
    return graph;
}
*/
#endif //NODES_HPP