#include <SFML/Graphics.hpp>
#include "../SFML_Functions/SFML_Functions.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>

float (*cross)(const Point&, const Point&) = SFUTIL::cross<float>;
float (*dot)(Point, Point) = SFUTIL::dot<float>;
bool (*getIntPt)(const Point&, const Point&, const Point&, const Point&, Point*) = SFUTIL::lineSegsIntersect<float>;

typedef unsigned int NODEID;
typedef Point Vec2;

Vec2 (*normalize)(const Vec2&) = SFUTIL::getUnitVec<float>;

uint next(uint i, uint sz)
{
    return i == sz - 1 ? 0 : i + 1;
}

uint last(uint i, uint sz)
{
    return i == 0 ? sz - 1 : i - 1;
}

float pseudoAngle(Point &P)
{
    float p = P.x / (fabsf(P.x) + fabsf(P.y));
    return (P.y > 0.f ? 3.f - p : 1.f + p);//0-4
}

struct Node
{
    Node(float x, float y, float _r) : pos(x,y), r(_r) {}
    Point pos;
    float r;
    //NODEID id;
    std::vector<NODEID> edges;//connected to... (not OWNED)
};//ie a circle
std::vector<Node> nodes;

//sort ccw from -> 0
struct AngleSort
{
    AngleSort(Point &p):base(p){}
    Point &base;
    bool operator()(NODEID a, NODEID b)
    {
        Point A = nodes[a].pos - base;
        Point B = nodes[b].pos - base;
        
        return pseudoAngle(A) < pseudoAngle(B);
    }
};

//undirected graph
void link(NODEID A, NODEID B)
{
    nodes[A].edges.push_back(B);
    nodes[B].edges.push_back(A);
}

NODEID nextNode(NODEID curNodeId, NODEID lastNodeId)
{
    Node &curNode = nodes[curNodeId];
    for(uint i = 0; i < curNode.edges.size(); i++)
    {
        if(curNode.edges[i] == lastNodeId)
        {
            return curNode.edges[next(i,curNode.edges.size())];
        }
    }
    throw;
    return -1;
}

//outer traverse...
//assumes starts at endnode...
void traverseGraph(NODEID start)
{
    //go ccw... so first to next... 
    NODEID curId = start;
    std::cout<<"At node "<<curId<<std::endl;
    if(!nodes[start].edges.size()) return;

    curId = nodes[start].edges[0];
    NODEID lastId = start;
    std::cout<<"At node "<<curId<<std::endl;
    //while(curId != start)
    do
    {
        NODEID nextId = nextNode(curId,lastId);
        lastId = curId;
        curId = nextId;
        std::cout<<"At node "<<curId<<std::endl;
    } while (curId != start);
    
}

sf::VertexArray lines;
std::vector<Point> points;

//I should step through this code to see where it messes up...
void drawOutline()
{
    lines.setPrimitiveType(sf::LineStrip);
    //start at 0 and go around...
    NODEID curId = 0;
    for(curId = 0; curId != nodes.size(); curId++)
        if(nodes[curId].edges.size() == 1) break;
//    NODEID curId = 0;
//    NODEID lastId = 1;
    NODEID lastId = nodes[curId].edges[0];
    while(true)
    {
        //start with left
        Node &curNode = nodes[curId];
        Node &lastNode = nodes[lastId];
        Vec2 Vl = lastNode.pos - curNode.pos;
        Vec2 vl_n(-Vl.y, Vl.x);
        vl_n = normalize(vl_n);
        NODEID nextId = nextNode(curId,lastId);
        Node &nextNode = nodes[nextId];
        Vec2 Vn = curNode.pos - nextNode.pos;
        Vec2 vn_n(-Vn.y, Vn.x);//.normalize()
        vn_n = normalize(vn_n);

        bool toLeft = cross(Vl,Vn) > 0;

        if(curNode.edges.size() == 1)//end node// draw endpts...
        {
            points.push_back(curNode.pos + vl_n * curNode.r);
            points.push_back(curNode.pos + vn_n * curNode.r);
            //switch 
            lastId = curId;
            curId = nextId;
            if(curId == 0) break;
            continue;
        }
        
        if(toLeft)//not a miter.. add 2 points
        {
            
            points.push_back(curNode.pos + vl_n * curNode.r);
            points.push_back(curNode.pos + vn_n * curNode.r);
        }
        else
        {
            //a miter, just add the 1 point
            Point pl1 = curNode.pos + vl_n * curNode.r;
            Point pl2 = lastNode.pos + vl_n * lastNode.r;
            Point pn1 = curNode.pos + vn_n * curNode.r;
            Point pn2 = nextNode.pos + vn_n * nextNode.r;
            Point intpt;
            getIntPt(pl1,pl2,pn1,pn2,&intpt);
            points.push_back(intpt);
        }

        lastId = curId;
        curId = nextId;
        if(curId == 0) break;
        
    }
}
/*
void traverseGraph(NODEID start)
{
    //go ccw... so first to next... 
    NODEID curId = start;
    std::cout<<"At node "<<curId<<std::endl;
    if(!nodes[start].edges.size()) return;

    curId = nodes[start].edges[0];
    NODEID lastId = start;
    std::cout<<"At node "<<curId<<std::endl;
    //while(curId != start)
    do
    {
        NODEID nextId = nextNode(curId,lastId);
        lastId = curId;
        curId = nextId;
        std::cout<<"At node "<<curId<<std::endl;
    } while (curId != start);
    
}*/

void drawOutlinePoints()
{
    lines.setPrimitiveType(sf::Lines);
    //for each node...
    //for each edge from that node...
    //there are 2 points
    for(uint i = 0; i < nodes.size(); i++)
    {
        Node &n = nodes[i];
        //for each edge...
        for(uint k = 0; k < n.edges.size(); k++)
        {
            //if(k!=1 || n.edges.size()!=3 ) continue;
            Node &e = nodes[n.edges[k]];
            Node &e_next = nodes[n.edges[next(k,n.edges.size())]];
            Node &e_last = nodes[n.edges[last(k,n.edges.size())]];

            Vec2 v0 = e.pos - n.pos;
            Vec2 vp = e_last.pos - n.pos;
            Vec2 va = e_next.pos - n.pos;

            Vec2 v0n(-v0.y, v0.x);
            v0n = normalize(v0n);
            if(n.edges.size() == 1)//no miter just end point...
            {
                points.push_back(n.pos - v0n * n.r);
                points.push_back(n.pos + v0n * n.r);
                continue;
            }

            Vec2 vpn(-vp.y, vp.x);
            vpn = normalize(vpn);
            Vec2 van(-va.y, va.x);
            van = normalize(van);

            //is it a miter point or a tan point?
            bool lastToLeft = cross(vp,v0) > 0.f;
            bool nextToLeft = cross(v0,va) > 0.f;

            if(!lastToLeft)//just regular...
            {
                //lines.append({n.pos,sf::Color::Blue});
                //lines.append({e_last.pos,sf::Color::Blue});
                //lines.append({n.pos,sf::Color::Black});
                //lines.append({e.pos,sf::Color::Black});
                lines.append({n.pos,sf::Color::Black});
                lines.append({n.pos - v0n * n.r,sf::Color::Black});
                points.push_back(n.pos - v0n * n.r);
            }
            else//miter
            {//BLOODY>>>> THINKING REQUIRED :(
                lines.append({n.pos,sf::Color::Black});
                lines.append({e.pos,sf::Color::Black});
                lines.append({n.pos,sf::Color::Black});
                lines.append({e_last.pos,sf::Color::Black});
                /*
                Vec2 vl = n.pos - e_last.pos;
                vl = normalize(vl);
                Vec2 vln(-vl.y,vl.x);
                Vec2 tangent = normalize(v0n + vpn);
                Vec2 miter(-tangent.y, tangent.x);
                float l = (n.r + e_last.r) / dot(miter,v0n);//normal
                miter *= l/2;*/
                Point pe1 = n.pos - v0n * n.r;
                Point pe2 = e.pos - v0n * e.r;
                Point pp1 = n.pos + vpn * n.r;
                Point pp2 = e_last.pos + vpn * e_last.r;
                Point intpt;
                getIntPt(pe1,pe2,pp1,pp2,&intpt); 
                lines.append({n.pos,sf::Color::Blue});
                lines.append({intpt,sf::Color::Blue});
                points.push_back(intpt);
            }
            if(!nextToLeft)//just regular...
            {
                //lines.append({n.pos,sf::Color::Green});
                //lines.append({e_next.pos,sf::Color::Green});
                //lines.append({n.pos,sf::Color::Yellow});
                //lines.append({e.pos,sf::Color::Yellow});
                lines.append({n.pos,sf::Color::Yellow});
                lines.append({n.pos + v0n * n.r,sf::Color::Yellow});
                points.push_back(n.pos + v0n * n.r);
            }
            else//miter
            {
                Vec2 tangent = normalize(van + v0n);
                Vec2 miter(-tangent.y, tangent.x);
                float l = (n.r + e_next.r) / dot(miter,van);
                miter *= l/2;
                //points.push_back(n.pos - miter);
            }
        }
    }
}

//////////////////visualizations...////////////////////
std::vector<sf::CircleShape> nodeCircles;
void drawNodes()
{
    sf::CircleShape C;
    C.setFillColor(sf::Color(255,0,0,64));
    for(auto &n : nodes)
    {
        C.setRadius(n.r);
        C.setOrigin(n.r,n.r);
        C.setPosition(n.pos);
        nodeCircles.push_back(C);
    }
}


std::vector<sf::CircleShape> pointCircles;
void drawPoints()
{
    sf::CircleShape C;
    C.setFillColor(sf::Color(0,255,0,64));
    C.setOrigin(5,5);
    C.setRadius(5);
    for(auto &p : points)
    {
        C.setPosition(p);
        pointCircles.push_back(C);
    }
}

///////////////////////
std::vector<sf::CircleShape> circles;

void genCircles(sf::FloatRect &&bounds)
{
    srand(time(NULL));
    sf::CircleShape circle;
    circle.setFillColor(sf::Color(255,0,0,128));

    //try and create a ground?  left and right
    /*
    {
        float x = -10;
        float y = 10;//rand() % (int)bounds.height + bounds.top;
        float r = rand() % 15 + 5;
        circle.setOrigin(r,r);
        circle.setRadius(r);
        circle.setPosition(x,y);
        circles.push_back(circle);
        x = 110;
        y = 10;//rand() % (int)bounds.height + bounds.top;
        r = rand() % 15 + 5;
        circle.setOrigin(r,r);
        circle.setRadius(r);
        circle.setPosition(x,y);
        circles.push_back(circle);
    }*/

    for(int i = 0; i < 25; i++)
    {
        float x = rand() % (int)bounds.width + bounds.left;
        float y = rand() % (int)bounds.height + bounds.top;
        float r = rand() % 15 + 5;
        circle.setOrigin(r,r);
        circle.setRadius(r);
        circle.setPosition(x,y);

        bool collided = false;
        for(auto &c : circles)
        {
            float r2 = c.getRadius() + r;
            float dx = c.getPosition().x - x;
            float dy = c.getPosition().y - y;

            if(r2 * r2 > (dx * dx + dy * dy))
            {
                collided = true;
                break;
            }
        }
        if(!collided)   
            circles.push_back(circle);
    }
}

struct Edge
{
    unsigned int a, b;
    float l2;
};

std::vector<Edge> edges;
std::vector<Edge> closeEdges;
sf::VertexArray edgeLines;
void genLines()
{
    for(uint i = 0; i < circles.size()-1; i++)
    {
        for(uint j = i+1; j < circles.size(); j++)
        {
            //create
            sf::Vector2f d = circles[i].getPosition() - circles[j].getPosition();
            float l2 = d.x * d.x + d.y * d.y;
            //append
            edges.push_back({i,j,l2});
        }
    }
    //sort...
    auto edgeSort = [](const Edge &E1, const Edge & E2) -> bool
    {
        return E1.l2 < E2.l2;
    };
    std::sort(edges.begin(),edges.end(),edgeSort);

//try Kruskal's algorithm here...
//no understand it.. copy pasta
    std::vector<uint> id;
    id.resize(edges.size());
    for(uint i = 0; i<id.size(); ++i) id[i] = i;

    auto root = [&](uint x) -> uint
    {
        while (id[x] != x)
        {
            id[x] = id[id[x]];
            x = id[x];
        }
        return x;
    };

    auto union1 = [&](uint x, uint y)
    {
        uint p = root(x);
        uint q = root(y);
        id[p] = id[q];
    };

    std::vector<bool> visited;
    std::vector<bool> addedEdge(edges.size(),false);
    uint numVisited = 0;
    visited.resize(circles.size(), false);
    for(uint i = 0; i < edges.size(); ++i)
    {
        if(root(edges[i].a) != root(edges[i].b))
        {
            union1(edges[i].a,edges[i].b);
            if(!visited[edges[i].a])
            {
                visited[edges[i].a] = true;
                numVisited++;
            }
            if(!visited[edges[i].b])
            {
                visited[edges[i].b] = true;
                numVisited++;
            }
            closeEdges.push_back(edges[i]);
            addedEdge[i]=true;
            if(numVisited == edges.size())
                break;
        }
    }
    //need to use (delauney type) edge list... then can just choose random edges from there...
    for(uint i = 0; i<addedEdge.size(); ++i)
    {
        if(addedEdge[i] == false) {closeEdges.push_back(edges[i]);
        break;}
    }


    std::cout<<"Ids are now:";
    for(auto &i : id) std::cout<<" "<<i;
    std::cout<<std::endl;

    //show
    edgeLines.setPrimitiveType(sf::Lines);
    for(auto &e : closeEdges)
    {
        edgeLines.append({circles[e.a].getPosition(),sf::Color::White});
        edgeLines.append({circles[e.b].getPosition(),sf::Color::White});
    }
}

int main()
{
//create it from our graph....
/*
    sf::RenderTexture target;
    target.create(640,640);//width height
    sf::View view;
    view.setSize(120,120);
    view.setCenter(50,50);
    target.setView(view);*/

    genCircles({0,0,100,100});
    genLines();

    //convert over
    for(auto &c : circles)
        nodes.emplace_back(c.getPosition().x,c.getPosition().y,c.getRadius());
    for(auto &e : closeEdges)
        link(e.a,e.b);
/*
    nodes.emplace_back(160,190,20);//0
    nodes.emplace_back(180,140,15);//1
    nodes.emplace_back(120, 60,40);//2
    nodes.emplace_back(190, 60,10);//3
    nodes.emplace_back( 60,130,20);//4
    nodes.emplace_back( 40,100,10);//5

    link(0,1);
    link(1,2);
    link(2,3);
    link(2,4);
    link(4,5);*/
    //link(1,3);

    for(auto &n : nodes)
    {
        if(n.edges.size()>2)
            std::sort(n.edges.begin(),n.edges.end(),AngleSort(n.pos));
    }
    //std::sort(edges.begin(),edges.end(),AS);
//print graph...
    for(uint i = 0; i<nodes.size(); i++)
    {
        std::cout<<"Node: "<<i<<" e: ";
        for(uint k = 0; k<nodes[i].edges.size(); k++)
            std::cout<<nodes[i].edges[k]<<" ";
        std::cout<<std::endl;
    }

    drawNodes();
    drawOutline();
//    drawOutlinePoints();
    drawPoints();
    traverseGraph(4);

    sf::VertexArray edgeLines;
    edgeLines.setPrimitiveType(sf::Lines);
    for(auto &n : nodes)
        for(auto &e : n.edges)
        {
            edgeLines.append({n.pos, sf::Color::Green});
            edgeLines.append({nodes[e].pos, sf::Color::Green});
        }

    for(auto &p : points)
        lines.append({p,sf::Color::Black});
    lines.append(lines[0]);

    sf::RenderWindow window(sf::VideoMode(640,640),"Graph");
    sf::Event event;
    sf::View view;
    view.setCenter(50,50);
    view.setSize(120,120);
    window.setView(view);
    CameraController camera(window,view);

    //view.rotate(33);

    while (window.isOpen())
    {
        window.clear(sf::Color(64,64,64));
        window.draw(lines);
        window.draw(edgeLines);
        for(auto &c : pointCircles) window.draw(c);
        for(auto &c : nodeCircles) window.draw(c);
        window.display();

        window.waitEvent(event);
        switch (event.type)
        {
        case sf::Event::Closed:
        case sf::Event::KeyPressed:
            window.close();
            break;
        default:
            break;
        }
        camera.handleEvent(event);
    }

    std::ofstream ofile("points");
    for(auto &p : points) ofile << "{" << p.x << "," << p.y << "}," << std::endl;
    return 0;
}