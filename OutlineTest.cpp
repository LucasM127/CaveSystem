#include <SFML/Graphics.hpp>
#include "OutlineGraph.hpp"
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

    //std::vector<GraphNode> graph = genGraph(K.nodes.size(), K.chosenEdges);
    OutlineGenerator S(K.nodes, K.chosenEdges);
    //GenOutlineStepper S(K.nodes, graph);

    ///////////DISPLAYING///////////////////

    sf::View view;
    view.setCenter(bounds.center());
    view.setSize(bounds.size());
    sf::RenderWindow window(sf::VideoMode(800,800),"MST?");
    sf::View defaultView = window.getView();
    window.setView(view);

    ////////////VISUALIZATIONS//////////////

    std::vector<sf::CircleShape> nodeShapes(NG.nodes.size());
    {
        sf::CircleShape c;
        c.setRadius(1.f);
        c.setOrigin(1.f,1.f);
        c.setFillColor(sf::Color(sf::Color(200,32,64,96)));
        for(auto &node : NG.nodes)
        {
            c.setPosition(node.pos);
            c.setScale(node.r,node.r);
            nodeShapes.push_back(c);
        }
    }

    std::vector<sf::Text> nodeIdTexts;
    sf::Font font;
    font.loadFromFile("DancingScript-Bold.ttf");
    for(unsigned int i = 0; i < NG.nodes.size(); ++i)
    {
        std::string string =  std::to_string(i);
        sf::Text text;
        text.setString(string);
        text.setFillColor(sf::Color::White);
        text.setFont(font);
        sf::Vector2i pos = window.mapCoordsToPixel(NG.nodes[i].pos);
        text.setPosition((float)pos.x, (float)pos.y);
        text.setCharacterSize(14);
        nodeIdTexts.push_back(text);
    }

    sf::VertexArray edgeLines(sf::Lines);
    for(auto &e : K.chosenEdges)
    {
        edgeLines.append({K.nodes[e.a].pos,sf::Color(128,255,128,128)});
        edgeLines.append({K.nodes[e.b].pos,sf::Color(128,255,128,128)});
    }

    std::vector<sf::CircleShape> linePointShapes;
    sf::CircleShape linePointShape;
    linePointShape.setFillColor(sf::Color(192,160,64,96));//more yellowy!
    linePointShape.setRadius(1.5f);
    linePointShape.setOrigin(1.5f,1.5f);

    sf::VertexArray outLine(sf::LineStrip);


    sf::Event event;
    while(window.isOpen())
    {
        window.clear(sf::Color(64,64,64));
        if(!S.isDone)
        {
        for(auto &shape : nodeShapes)
            window.draw(shape);
        window.draw(edgeLines);
        for(auto &shape : linePointShapes)
            window.draw(shape);
        }
        else
            window.draw(outLine);
        window.setView(defaultView);
        for(auto &text : nodeIdTexts)
            window.draw(text);
        window.setView(view);
        window.display();

        window.waitEvent(event);
        if(event.type == sf::Event::Closed)
            window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Escape)
                window.close();
            if(!S.isDone)
            {
                std::cout<<"at node "<<S.curId<<"\n";
                S.step();
                linePointShapes.clear();
                for(auto &point : S.points)
                {
                    linePointShape.setPosition(point);
                    linePointShapes.push_back(linePointShape);
                }

                if(S.isDone)
                {
                    for(auto &point : S.points)
                    {
                        outLine.append({point, sf::Color(224,224,192,128)});
                    }
                    outLine.append(outLine[0]);
                }
            }//!done
        }
    }

    return 0;
}