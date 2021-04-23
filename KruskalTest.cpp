#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include "Kruskal.hpp"
#include "NodeGenerator.hpp"
#include <iostream>

//TODO: draw Node ID Text in test framework

void updateLines(const KruskalMST &K, sf::VertexArray &consideredLine, sf::VertexArray &chosenLines)
{
    consideredLine[0].position = K.nodes[K.edges[K.curEdgeId].a].pos;
    consideredLine[1].position = K.nodes[K.edges[K.curEdgeId].b].pos;

    //chosenLines.clear();
    //for(auto &e : K.chosenEdges)
    {
        chosenLines.append(consideredLine[0]);
        chosenLines.append(consideredLine[1]);
//        chosenLines.append({K.nodes[e.a].pos, sf::Color(0,255,0,128)});
//        chosenLines.append({K.nodes[e.b].pos, sf::Color(0,255,0,128)});
    }
}

int main()
{
    //generate Min Spanning Tree using kruskalls algorithm as I have done previously....
    srand(time(nullptr));

    //bounds
    Bounds2D bounds(0.f, 200.f, 0.f, 200.f);
    NodeGenerator NG;
    unsigned int numNodes = 15;
    NG.generate(bounds, numNodes, 2.f, 10.f);
    std::vector<sf::CircleShape> nodeShapes(NG.nodes.size());
    {
        sf::CircleShape c;
        c.setRadius(1.f);
        c.setOrigin(1.f,1.f);
        c.setFillColor(sf::Color(sf::Color(200,32,64,160)));
        for(auto &node : NG.nodes)
        {
            c.setPosition(node.pos);
            c.setScale(node.r,node.r);
            nodeShapes.push_back(c);
        }
    }

    KruskalMST K(NG.nodes);
    
    sf::VertexArray edgeLines(sf::Lines);
    for(auto &e : K.edges)
    {
        edgeLines.append({K.nodes[e.a].pos,sf::Color(128,128,128,32)});
        edgeLines.append({K.nodes[e.b].pos,sf::Color(128,128,128,32)});
    }

    sf::VertexArray consideredEdgeLine(sf::Lines, 2);
    consideredEdgeLine[0].color = consideredEdgeLine[1].color = sf::Color(255,255,0,128);

    sf::VertexArray chosenLines(sf::Lines);
    updateLines(K, consideredEdgeLine, chosenLines);
    
    sf::View view;
    view.setCenter(bounds.center());
    view.setSize(bounds.size());
    sf::RenderWindow window(sf::VideoMode(800,800),"MST?");
    window.setView(view);

    glLineWidth(2.f);
    bool showConsideredLine = true;
    sf::Event event;
    while (window.isOpen())
    {
        window.clear(sf::Color(64,64,64));
        for(auto &shape : nodeShapes)
            window.draw(shape);
        window.draw(edgeLines);
        window.draw(chosenLines);
        if(showConsideredLine)
            window.draw(consideredEdgeLine);
        window.display();

        window.waitEvent(event);
        if(event.type == sf::Event::Closed)
            window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Escape)
                window.close();
            showConsideredLine = K.stepMST();
            updateLines(K, consideredEdgeLine, chosenLines);
            for(size_t i = 0; i < K.chosenEdges.size(); ++i)
            {
                std::cout<<K.edgeIds[i]<<" ";
            }
            std::cout<<std::endl;
        }
    }

    return 0;
}