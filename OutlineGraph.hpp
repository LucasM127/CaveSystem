#ifndef OUTLINE_GRAPH_HPP
#define OUTLINE_GRAPH_HPP

#include "Nodes.hpp"

struct GraphNode
{
    std::vector<std::pair<NODEID,EDGEID>> neighbours;
};

//adjacency list representation
inline std::vector<GraphNode> genGraph(const unsigned int numNodes, const std::vector<Edge> &edges)
{
    std::vector<GraphNode> graph(numNodes);
    for(EDGEID i = 0; i < edges.size(); ++i)
    {
        graph[edges[i].a].neighbours.emplace_back(edges[i].b,i);
        graph[edges[i].b].neighbours.emplace_back(edges[i].a,i);
    }

    return graph;
}

struct EdgeTangentPoints
{
    Point p[4];
};

//Input = Kruskal Output
class OutlineGenerator
{
public:
    OutlineGenerator(const std::vector<Node2D> &_nodes, const std::vector<Edge> &_edges);
    void step();
    void generate();

    const std::vector<Node2D> &nodes;
    const std::vector<Edge> &edges;
    
    std::vector<GraphNode> graph;
    std::vector<Point> points;
    std::vector<EdgeTangentPoints> tangentPoints;

    bool isDone;
    NODEID curId, lastId;
private:
    unsigned int m_numVisits;
    unsigned int m_totalNumVisits;

    NODEID addPoint();
};

#endif //OUTLINE_GRAPH_HPP