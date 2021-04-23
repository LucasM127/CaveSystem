//This Test Demonstrates using 1/cos(30) as a factor for minimal separation distance
#include <SFML/Graphics.hpp>
#include "../Nodes.hpp"
#include "../Bounds2D.hpp"
#include "../SFVectorFuncs.hpp"
#include <cmath>

std::ostream &operator<<(std::ostream &os, const sf::Vector2f &p)
{
    return os<<p.x<<" "<<p.y;
}

const float degToRad = M_PI / 180.f;
const float sin60 = sinf(degToRad * 60.f);
const float cos60 = cosf(degToRad * 60.f);
const float sin30 = sinf(degToRad * 30.f);
const float cos30 = cosf(degToRad * 30.f);
const float factor = 1.f / cosf(degToRad * 30.f);//  1.15470053838f;//1 / cos30

//Bounds
const float radius = 1.f;
const float diameter = 2.f * radius;
const float width = diameter * factor + diameter;
const float height = 2 * diameter;

//circles
const Point c1(radius,radius);
const Point c2(width - radius, radius);
const Point c3(width/2.f, 3*radius);

//Lines of tangency

//y = mx+b
//x = width/2
//y = diameter
Vec2 v3(1.f,0.f);
Vec2 v2(sin30,cos30);
Vec2 v1(-sin30,cos30);
Vec2 v1n(-v1.y,v1.x);//ccw90
Vec2 v2n(-v2.y,v2.x);
Vec2 v3n(-v3.y,v3.x);

//Points of tangency
const Point t11 = c1 - v1n * radius;
const Point t21 = c2 + v2n * radius;
const Point t31 = c3 - v3n * radius;

//y=mx+b form equation of lines
const float m1 = -sin60 / cos60;
const float m2 = sin60 / cos60;
const float m3 = 0.f;

//b=y-mx
//const float b1 = t11.y - m1 * t11.x;
const float b1 = t11.y - m1 * t11.x;//diameter - (m2 * width/2.f);
const float b2 = t21.y - m2 * t21.x;// diameter - (m3 * width/2.f);

const Point t1a((0.f-b1)/m1,0.f);
const Point t1b((height-b1)/m1,height);
const Point t2a((0.f-b2)/m2,0.f);
const Point t2b((height-b2)/m2,height);
const Point t3a(0.f,diameter);
const Point t3b(width,diameter);

//tesselate a plane ? radius r
int main()
{
    std::vector<Node2D> nodes;

    Bounds2D bounds(0.f,width,0.f,height);
    float aspectRatio = width/height;
    unsigned int screenHeight = 720.f;
    unsigned int screenWidth = float(screenHeight)*aspectRatio;

    std::vector<sf::CircleShape> circleShapes(3);
    circleShapes[0].setRadius(radius);
    circleShapes[0].setOrigin(radius,radius);
    circleShapes[0].setFillColor(sf::Color::Transparent);
    circleShapes[0].setOutlineThickness(-radius/180.f);
    circleShapes[0].setOutlineColor(sf::Color::Black);
    circleShapes[0].setPosition(c1);

    circleShapes[1].setRadius(radius);
    circleShapes[1].setOrigin(radius,radius);
    circleShapes[1].setFillColor(sf::Color::Transparent);
    circleShapes[1].setOutlineThickness(-radius/180.f);
    circleShapes[1].setOutlineColor(sf::Color::Black);
    circleShapes[1].setPosition(c2);

    circleShapes[2].setRadius(radius);
    circleShapes[2].setOrigin(radius,radius);
    circleShapes[2].setFillColor(sf::Color::Transparent);
    circleShapes[2].setOutlineThickness(-radius/180.f);
    circleShapes[2].setOutlineColor(sf::Color::Black);
    circleShapes[2].setPosition(c3);

    sf::VertexArray tangentLines(sf::Lines, 6);
    tangentLines[0].color = tangentLines[1].color = sf::Color::Black;
    tangentLines[2].color = tangentLines[3].color = sf::Color::Black;
    tangentLines[4].color = tangentLines[5].color = sf::Color::Black;
    tangentLines[0].position = t1a;
    tangentLines[1].position = t1b;
    tangentLines[2].position = t2a;
    tangentLines[3].position = t2b;
    tangentLines[4].position = t3a;
    tangentLines[5].position = t3b;

    sf::RenderWindow window(sf::VideoMode(screenWidth,screenHeight),"Circles",sf::Style::Close);
    sf::View view;
    view.setCenter(bounds.center());
    view.setSize(bounds.size().x, -bounds.size().y);
    window.setView(view);

    sf::Event event;
    while(window.isOpen())
    {
        window.clear(sf::Color::White);
        for(const auto &c : circleShapes)
            window.draw(c);
        window.draw(tangentLines);
        window.display();

        window.waitEvent(event);
        if(event.type == sf::Event::Closed)
            window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Escape)
                window.close();
        }
        /*if(event.type == sf::Event::Resized)
        {
            //ensure height prop to width
            float winAspectRatio = float(event.size.width)/float(event.size.height);
            float sizeX = 1;
            float sizeY = 1;
            float posX = 0; 
            float posY = 0;
            if(winAspectRatio < aspectRatio)
            {//taller
                sizeY = winAspectRatio/aspectRatio;
                posX = (1-sizeY)/2.f;
            }
            else
            {//wider
                sizeX = aspectRatio/winAspectRatio;
                posY = (1-sizeX)/2.f;
            }
            view.setViewport({posX,posY, sizeX, sizeY});
            window.setView(view);
        }*/
    }

    return 0;
}