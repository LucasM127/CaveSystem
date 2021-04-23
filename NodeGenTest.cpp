#include "NodeGenerator.hpp"

#include <SFML/Graphics.hpp>
#include <cassert>

void updateShapes(std::vector<sf::CircleShape> &nodeShapes, const std::vector<Node2D> &nodes)
{
    assert(nodeShapes.size() == nodes.size());
    for(size_t i = 0; i < nodeShapes.size(); ++i)
    {
        nodeShapes[i].setPosition(nodes[i].pos);
        nodeShapes[i].setScale(nodes[i].r,nodes[i].r);
    }
}

//Transfer my camera over properly :(
void updateBounds(const std::vector<Node2D> &nodes, Bounds2D &b)
{
    b.min_x = b.min_y = 100.f;
    b.max_x = b.max_y = -100.f;
    for(auto &n : nodes)
    {
        if(b.min_x < (n.pos.x - n.r)) b.min_x = (n.pos.x - n.r);
        if(b.min_y < (n.pos.y - n.r)) b.min_y = (n.pos.y - n.r);
        if(b.max_x > (n.pos.x + n.r)) b.max_x = (n.pos.x + n.r);
        if(b.max_y > (n.pos.y + n.r)) b.max_y = (n.pos.y + n.r);
    }
}

int main()
{
    srand(time(nullptr));

    //bounds
//    Bounds2D bounds(-100.f, 300.f, -100.f, 300.f);
    Bounds2D bounds(0.f, 200.f, 0.f, 200.f);
    NodeGenerator NG;
    unsigned int numNodes = 100;//15;
    //NG.create(bounds, numNodes, 2.f, 10.f);
    NG.generate(bounds, numNodes, 2.f, 10.f);
    std::vector<sf::CircleShape> nodeShapes;
    nodeShapes.resize(numNodes);
    for(auto &shape : nodeShapes)
    {
        shape.setFillColor(sf::Color(sf::Color(200,32,64,160)));
        shape.setRadius(1.f);
        shape.setOrigin(1.f,1.f);
    }

    updateShapes(nodeShapes, NG.nodes);

    sf::View view;
    view.setCenter(bounds.center());
    view.setSize(bounds.size());

    sf::RenderWindow window(sf::VideoMode(800,800), "Caves?");
    window.setView(view);

    sf::Event event;
    while(window.isOpen())
    {
        window.clear(sf::Color(64,64,64));
        for(const auto &c : nodeShapes)
            window.draw(c);
        window.display();

        window.waitEvent(event);
        if(event.type == sf::Event::Closed)
            window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Escape)
                window.close();
            //NG.updateStep();
            //updateShapes(nodeShapes, NG.nodes);
            //updateBounds(NG.nodes, bounds);
            //view.setCenter(bounds.center());
            //view.setSize(bounds.size());
            //window.setView(view);
        }
    }

    return 0;
}