#include <SFML/Graphics.hpp>
#include <iostream>
#include "SFVectorFuncs.hpp"

//REFERENCE
//http://extremelearning.com.au/evenly-distributing-points-in-a-triangle/

//draw lines...
//like in video.  nice arrows
void genArrow(sf::VertexArray &V, const sf::Color &color)
{
    V.clear();
    V.setPrimitiveType(sf::Triangles);

    //origin 0,0 facing up width 1, height 1
    V.append({{-0.3f, 0.f},color});
    V.append({{ 0.3f, 0.f},color});
    V.append({{-0.3f,-0.6f},color});

    V.append({{-0.3f,-0.6f},color});
    V.append({{ 0.3f, 0.f},color});
    V.append({{ 0.3f,-0.6f},color});//rectangle base

    V.append({{ -1.f,-0.6f},color});
    V.append({{  1.f,-0.6f},color});
    V.append({{  0.f,-1.0f},color});
}

struct BaryCentricCoords
{
    float a,b,c;
};

using Point = sf::Vector2f;

struct Triangle
{
    Point p[3];
};

//method 1
BaryCentricCoords convert(const Point &a, const Point &b, const Point &c, const Point &p)
{
    float s1 = c.y - a.y;
    float s2 = c.x - a.x;
    float s3 = b.y - a.y;
    float s4 = p.y - a.y;

    float w1 = (a.x * s1 + s4 * s2 - p.x * s1) / (s3 * s2 - (b.x - a.x) * s1);
    float w2 = (s4 - w1 * s3) / s1;
    float w3 = 1 - w1 - w2;

    return {w1,w2,w3};
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(720,480),"Fence");

    unsigned int tri_index = 0;
    Triangle T;
    sf::VertexArray triangleShape(sf::Triangles);

    sf::VertexArray w1Shape;
    genArrow(w1Shape, sf::Color::Green);
    sf::VertexArray w2Shape;
    genArrow(w1Shape, sf::Color::Blue);
    sf::VertexArray w3Shape;
    genArrow(w1Shape, sf::Color::Red);
    sf::Transform t1, t2, t3;
    float l1, l2, l3;

    sf::Font font;
    font.loadFromFile("DancingScript-Bold.ttf");
    sf::Text text1,text2,text3;
    text1.setPosition(10,10);
    text2.setPosition(10,40);
    text3.setPosition(10,70);
    text1.setFont(font);
    text2.setFont(font);
    text3.setFont(font);
    text1.setFillColor(sf::Color::White);
    text2.setFillColor(sf::Color::White);
    text3.setFillColor(sf::Color::White);


    sf::Event event;
    sf::Vector2f mousePos;

    auto update = [&]()
    {
        BaryCentricCoords BC = convert(T.p[0], T.p[1], T.p[2], mousePos);
        t1.scale(5.f, BC.a * l1);
        t2.scale(5.f, BC.b * l2);
        t3.scale(5.f, BC.c * l3);
        std::string string;
        string = "A: " + std::to_string(BC.a);
        text1.setString(string);
        string = "B: " + std::to_string(BC.b);
        text2.setString(string);
        string = "C: " + std::to_string(BC.c);
        text3.setString(string);
    };

    while (window.isOpen())
    {
        window.clear(sf::Color(48,32,64));
        window.draw(triangleShape);
        window.draw(text1);
        window.draw(text2);
        window.draw(text3);
        if(tri_index == 3)
        {
            window.draw(w1Shape,t1);
            window.draw(w2Shape,t2);
            window.draw(w3Shape,t3);
        }
        window.display();
        window.waitEvent(event);
        if(event.type == sf::Event::Closed)
            window.close();
        if(event.type == sf::Event::KeyPressed)
            window.close();
        if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
        {
            if(tri_index < 3)
            {
                T.p[tri_index] = mousePos;
                if(tri_index == 0)      triangleShape.append({T.p[0], sf::Color::Green});
                else if(tri_index == 1) triangleShape.append({T.p[1], sf::Color::Blue});
                else                    triangleShape.append({T.p[2], sf::Color::Red});
                ++tri_index;
                if(tri_index == 3)
                {
                    for(uint i = 0; i < w1Shape.getVertexCount(); ++i) w1Shape[i].position = w1Shape[i].position + T.p[0];
                    for(uint i = 0; i < w2Shape.getVertexCount(); ++i) w2Shape[i].position = w2Shape[i].position + T.p[1];
                    for(uint i = 0; i < w3Shape.getVertexCount(); ++i) w3Shape[i].position = w3Shape[i].position + T.p[2];
                    l1 = SFUTIL::length<float>(T.p[1]-T.p[0]);
                    l2 = SFUTIL::length<float>(T.p[2]-T.p[0]);
                    l3 = SFUTIL::length<float>(T.p[2]-T.p[1]);
                }
            }
        }
        if(event.type == sf::Event::MouseMoved)
        {
            mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if(tri_index == 3)
            {
                update();
            }
        }
    }
    
    return 0;
}