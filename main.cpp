#include <SFML/Graphics.hpp>
//#include "OutlineGraph.hpp"
#include "NodeGenerator.hpp"
#include "Kruskal.hpp"
#include <iostream>

int main()
{
    srand(time(nullptr));
    Bounds2D bounds(0.f, 200.f, 0.f, 200.f);
    NodeGenerator NG;
    unsigned int numNodes = 15;
    NG.generate(bounds, numNodes, 2.f, 10.f);

    KruskalMST K(NG.nodes);
    K.genMST();

    std::vector<GraphNode> graph = genGraph(K.nodes.size(), K.chosenEdges);

    //traverse graph is the same too...
}