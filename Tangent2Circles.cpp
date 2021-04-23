//https://gieseanw.wordpress.com/2012/10/21/a-comprehensive-step-by-step-tutorial-to-computing-dubins-paths/
//plus figuring it out for my self the link helped a lot as my brain was mushed so simple in retrospect... just thinking that way
#include <SFML/Graphics.hpp>
#include "SFVectorFuncs.hpp"

//REDO MY OUTLINE GENERATOR WITH THIS ALGORITHM

int main()
{
    srand(time(nullptr));
    //place two random circles
    sf::CircleShape C1, C2;
    float r1 = 1 + rand()%10;
    float r2 = 1 + rand()%10;
    //at same y ...
    sf::Vector2f c1(20.f, 30.f);//80x60
    sf::Vector2f c2(60.f, 30.f);

    C1.setRadius(r1);
    C1.setOrigin(r1,r1);
    C1.setPosition(c1);
    C1.setFillColor(sf::Color(255,128,128,128));

    C2.setRadius(r2);
    C2.setOrigin(r2,r2);
    C2.setPosition(c2);
    C2.setFillColor(sf::Color(255,128,128,128));

    //draw Tangent Line
    sf::Vector2f p1, p2, p3, p4;//points of tangency external above

    sf::Vector2f v12 = c2 - c1;
    float dist = SFUTIL::length(v12);
    sf::Vector2f v12_unit = v12/dist;
    
    float cosTheta = (r1 - r2)/dist;
    float sinTheta = sqrtf(1-cosTheta*cosTheta);

    sf::Vector2f t12n_unit;
    t12n_unit.x = v12_unit.x * cosTheta - v12_unit.y * sinTheta;
    t12n_unit.y = v12_unit.x * sinTheta + v12_unit.y * cosTheta;

    //rotate other way .. it works YAY
    sf::Vector2f t2;
    t2.x = v12_unit.x * cosTheta + v12_unit.y * sinTheta;
    t2.y = -v12_unit.x * sinTheta + v12_unit.y * cosTheta;

    p1 = c1 + (t12n_unit * r1);
    p2 = c2 + (t12n_unit * r2);
    p3 = c1 + (t2 * r1);
    p4 = c2 + (t2 * r2);

    sf::VertexArray tangentLine(sf::Lines, 8);
    tangentLine[0].color = tangentLine[1].color = sf::Color::Black;
    tangentLine[2].color = tangentLine[3].color = sf::Color::Green;
    tangentLine[4].color = tangentLine[5].color = sf::Color::Black;
    tangentLine[6].color = tangentLine[7].color = sf::Color::Black;
    tangentLine[0].position = p1;
    tangentLine[1].position = p2;
    tangentLine[2].position = p3;
    tangentLine[3].position = p4;
    //updown
    tangentLine[4].position = {c1.x, 0};
    tangentLine[5].position = {c1.x, 60};
    tangentLine[6].position = {c2.x, 0};
    tangentLine[7].position = {c2.x, 60};

    sf::RenderWindow window(sf::VideoMode(800,600),"Tangent Circles");
    sf::View view;
    view.setCenter(40.f,30.f);
    view.setSize(80.f,-60.f);
    window.setView(view);

    sf::Event event;
    while (window.isOpen())
    {
        window.clear(sf::Color(192,192,192));
        window.draw(C1);
        window.draw(C2);
        window.draw(tangentLine);
        window.display();
        window.waitEvent(event);
        if(event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
            window.close();
    }

    return 0;
}