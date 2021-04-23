#ifndef OUTLINE_GRAPH_HPP
#define OUTLINE_GRAPH_HPP

#include "Nodes.hpp"

std::vector<Point> genOutline(const std::vector<Node2D> &nodes, std::vector<GraphNode> &graph);

class GenOutlineStepper
{
public:
    GenOutlineStepper(const std::vector<Node2D> &_nodes, std::vector<GraphNode> &_graph);
    void step();

    const std::vector<Node2D> &nodes;
    std::vector<GraphNode> &graph;
    std::vector<Point> points;
    bool isDone;
    ID curId, lastId;
private:
    unsigned int m_numVisits;
    unsigned int m_totalNumVisits;
};

#endif //OUTLINE_GRAPH_HPP