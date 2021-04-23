//Create Outline Border Rectangle Triangle Regions

//INPUT: PolygonShape
//Output: Bordered Polygon ... trapezoid list (2 triangles) or triangle list
//Output: Point List or some way to ... start end point...
//Then line snap each line segment with points generated in corresponding trapezoidal region(s)

//Observations:
//The sharpest convex corner is 90 degrees.
//Likely due to the use of circles for the nodes.
//Occasionally at full width w self collides slightly ... something about node placement.  3 times from a sample of 110 tries.
#include <SFML/Graphics.hpp>
#include "OutlineGraph.hpp"
#include "NodeGenerator.hpp"
#include "Kruskal.hpp"
#include <iostream>
#include <cassert>
#include "SFVectorFuncs.hpp"
/*
inline ID next(ID i, ID sz)
{
    return i == sz - 1 ? 0 : i + 1;
}

inline ID last(ID i, ID sz)
{
    return i == 0 ? sz - 1 : i - 1;
}
*/
//YAY

int main()
{
    std::cout<<"Seed:\n";
    unsigned int seed = time(nullptr);
    std::cout<<seed<<"\n";
    srand(seed);
    Bounds2D bounds(0.f, 200.f, 0.f, 200.f);//(-100.f,300.f,-100.f,300.f);
//    Bounds2D bounds(-50.f,250.f,-50.f,250.f);//(0.f, 200.f, 0.f, 200.f);
    std::vector<Point> outline;
    std::vector<sf::Text> nodeIdTexts;
    sf::Font font;
    font.loadFromFile("DancingScript-Bold.ttf");

    sf::View view;
    view.setCenter(bounds.center());
    view.setSize(bounds.size());
    sf::RenderWindow window(sf::VideoMode(800,800),"MST?");
    //sf::View defaultView = window.getDefaultView()
    window.setView(view);

    //Get outline shape dont care about the other data in this test    
    {
    NodeGenerator NG;
    unsigned int numNodes = 15;
    NG.generate(bounds, numNodes, 10.f, 10.f);//1 vs 2
    KruskalMST K(NG.nodes);
    K.genMST();
    //std::vector<GraphNode> graph = genGraph(K.nodes, K.chosenEdges);
    //outline = genOutline(K.nodes, graph);
    OutlineGenerator OG(K.nodes,K.chosenEdges);
    OG.generate();
    outline = OG.points;
    assert(outline.size()>0);
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
    }

    sf::VertexArray outlineShape(sf::LineStrip);
    for(unsigned int i = 0; i < outline.size(); ++i)
    {
        outlineShape.append({outline[i],sf::Color::White});
    }
    outlineShape.append(outlineShape[0]);

    sf::VertexArray border(sf::Triangles);

    std::vector<Point> miterOutline(outline.size());

    {
    float w = 5.f;//10.f;
    unsigned int k = 0;
    unsigned int j = outline.size() - 1;
    unsigned int i = last(j, outline.size());
    while(k < outline.size())
    {
        const Point &p_last = outline[i];
        const Point &p = outline[j];
        const Point &p_next = outline[k];

        Vec2 v01 = p_last - p;
        Vec2 v01n = SFUTIL::getUnitVec<float>({-v01.y, v01.x});
        Vec2 v12 = p - p_next;
        Vec2 v12n = SFUTIL::getUnitVec<float>({-v12.y, v12.x});
        bool isConvex = SFUTIL::cross<float>(v01,v12) > 0.f;//float point an issue?

        Vec2 miterVec = v01n + v12n;

        if(isConvex)//else extends too far out and can collide
            miterVec = SFUTIL::getUnitVec<float>(miterVec) * w;
        else
        {
            float factor = 1 + SFUTIL::dot(v01n,v12n);
            miterVec = miterVec * w / factor;//bisector vector factor doodady
        }
        
        miterOutline[j] = (p + miterVec);

        i = j;
        j = k;
        ++k;
    }
    }
    {
    unsigned int i = outline.size() - 1;
    unsigned int j = 0;
    while(j < outline.size())
    {
        sf::Color color;
//        color = sf::Color(128,255,128,96);
        color.a = 96;
        color.r = rand()%128 + 128;
        color.g = rand()%128 + 128;
        color.b = rand()%128 + 128;

        border.append({miterOutline[i], color});
        border.append({outline[i], color});
        border.append({outline[j], color});

        //color.r = rand()%128 + 128;
        //color.g = rand()%128 + 128;
        //color.b = rand()%128 + 128;

        border.append({miterOutline[i], color});
        border.append({outline[j], color});
        border.append({miterOutline[j], color});

        i = j;
        ++j;
    }
    }

    sf::Event event;
    while(window.isOpen())
    {
        window.clear(sf::Color(64,64,64));
        window.draw(outlineShape);
        window.draw(border);
        window.setView(window.getDefaultView());
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
            //
        }
    }

    return 0;
}