#include "NodeGenerator.hpp"
#include <random>

namespace
{

float randFloat(int granularity, float min, float max)
{
    float range = max - min;
    return (float(rand()%granularity)/float(granularity)) * range + min;
}

inline float length(const sf::Vector2f &V)
{
    return sqrtf(V.x*V.x + V.y*V.y);
}

//SHOOTS UP DOWN A BUNCH DON"T LIKE THAT
sf::Vector2f divide(const sf::Vector2f &V, float f)
{
    if(f < 0.0001f)
    {
        //random direction
        float x = cosf(randFloat(360,0,2.f*M_PI));
        float y = sinf(randFloat(360,0,2.f*M_PI));
        return sf::Vector2f(x,y);
    }
    return V/f;
}
    
} // namespace

void NodeGenerator::generate(const Bounds2D &bounds, unsigned int numNodes, float minRadius, float maxRadius)
{
    create(bounds, numNodes, minRadius, maxRadius);
    
    while(!amDone)
        updateStep();
}

//IF I USE A FACTOR OF 1.14 with all nodes r = maxRadius, seems to be the smallest number I see no self intersections
//of border with, assuming a border all the way around of maxRadius.  1.13 I see self collisions.  But not 1.14 (yet)
//Not sure why or what mathematical proof would cause this to be.
void NodeGenerator::create(const Bounds2D &bounds, unsigned int numNodes, float minRadius, float maxRadius)
{
    nodes.clear();
    amDone = false;
    //1.13 intersections
    //1.14 good? wonder why... 1 + root 2/10 ???
    //1.189207115f good//1.15470053838f
    m_clearDist = maxRadius *3.f* 1.15470053838f;
//    m_clearDist = maxRadius *4.f* 1.15470053838f;// 1.14128f;//maxRadius * 4.f * 1.14f;//41421356237f;// * 1.4128f;//vs 2 ??? why roto 2
    
    //randomly sized
    sf::Vector2f center = bounds.center();
    for(unsigned int i = 0; i < numNodes; ++i)
    {
        //float r = maxRadius;// randFloat(100, minRadius, maxRadius);
        float r = randFloat(100, minRadius, maxRadius);
        nodes.emplace_back(center, r);
    }
}

//clear distance = 10 max radius = 10
//kinda like gravity. each node exerts gravity on other nodes f(dist)
void NodeGenerator::updateStep()
{
    amDone = true;
    std::vector<sf::Vector2f> offsets;
    offsets.resize(nodes.size(), sf::Vector2f(0.f,0.f));

    //calculate repulsion directions n^2 algorithm
    for(size_t i = 0; i < nodes.size(); ++i)
        for(size_t j = i+1; j < nodes.size(); ++j)
        {
            auto &nodeA = nodes[i];
            auto &nodeB = nodes[j];

            sf::Vector2f AB = nodeB.pos - nodeA.pos;
            float dist = length(AB);
            sf::Vector2f ABnormalized = divide(AB, dist);
            //if(dist == 0.f) dist = 1.f;
            if(dist < 1.f) dist = 1.f;
            if(dist > m_clearDist) continue;
            sf::Vector2f offsetAB = ABnormalized * (1.f/dist);
            sf::Vector2f offsetBA = -offsetAB;
            offsets[j] += offsetAB;
            offsets[i] += offsetBA;
            amDone = false;
        }

    //apply
    for(size_t i = 0; i < nodes.size(); ++i)
    {
        nodes[i].pos = nodes[i].pos + offsets[i];
    }
}
