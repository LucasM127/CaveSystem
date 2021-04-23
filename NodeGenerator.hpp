#ifndef NODE_GENERATOR_HPP
#define NODE_GENERATOR_HPP

#include "Bounds2D.hpp"
#include "Nodes.hpp"

class NodeGenerator
{
public:
    NodeGenerator(){}
    void generate(const Bounds2D &bounds, unsigned int numNodes, float minRadius, float maxRadius);

    void create(const Bounds2D &bounds, unsigned int numNodes, float minRadius, float maxRadius);
    void updateStep();

    std::vector<Node2D> nodes;
private:
    bool amDone;
    float m_clearDist;
};

#endif //NODE_GENERATOR_HPP