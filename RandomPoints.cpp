#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

//http://extremelearning.com.au/evenly-distributing-points-in-a-triangle/

const float g_r = 1.3247195572f;
const float g_x = 1.f/g_r;
const float g_y = 1.f/(g_r*g_r);

float n = 0.f;

//in range x = [0-1], y = [0-1]
sf::Vector2f getNextPoint()
{
    ++n;
    float x = fmodf(n*g_x,1.f);
    float y = fmodf(n*g_y,1.f);
    if((x + y) > 1.f)//w3 = 1 - w1 - w2
    {
        std::cout<<"x y "<<x<<" "<<y<<" ";
        x = 1.f - x;
        y = 1.f - y;
        std::cout<<x<<" "<<y<<std::endl;
    }
    return sf::Vector2f(x,y);
}

//Cache these points
//Calculate area of each boundary triangle
//place ~n points dependant on area (randomly)

int main()
{
    //random points from 0-1 0-1
    //window view -0.1 - 1.1
    sf::RenderWindow window(sf::VideoMode(800,800),"Distribution?");
    sf::View view;
    view.setCenter(0.5f,0.5f);
    view.setSize(1.1f,1.1f);
    window.setView(view);

    std::vector<sf::CircleShape> pointShapes;
    sf::CircleShape pointShape;
    pointShape.setFillColor(sf::Color(192,96,64,128));
    pointShape.setRadius(0.01f);
    pointShape.setOrigin(0.01f,0.01f);

    sf::Event event;
    while (window.isOpen())
    {
        window.clear(sf::Color(32,32,32));
        for(auto &point : pointShapes)
            window.draw(point);
        window.display();
        window.waitEvent(event);
        if(event.type == sf::Event::Closed)
            window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Escape)
                window.close();
            pointShape.setPosition(getNextPoint());
            pointShapes.push_back(pointShape);
            
        }
    }
    

    return 0;
}