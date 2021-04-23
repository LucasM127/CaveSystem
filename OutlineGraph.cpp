#include "OutlineGraph.hpp"
#include <algorithm>
#include "SFGeometricFuncs.hpp"//lineSegsIntersect

#include <iostream>

namespace
{

//maybe should have just done it each time around instead computationally.... :(

//Refer below for high level math explanation. (namely the cosTheta = (r1-r2)/dist part and how it relates to tangent lines between circles)
//https://gieseanw.wordpress.com/2012/10/21/a-comprehensive-step-by-step-tutorial-to-computing-dubins-paths/
std::vector<EdgeTangentPoints> calcTangentPoints(const std::vector<Edge>& edges, const std::vector<Node2D> &nodes)
{
    std::vector<EdgeTangentPoints> tangentPoints(edges.size());
    for(EDGEID i = 0; i < edges.size(); ++i)
    {//Calculate tangent line points from node circle 'a' to 'b' and 'b' to 'a'
        const Point c1 = nodes[edges[i].a].pos;
        const float r1 = nodes[edges[i].a].r;

        const Point c2 = nodes[edges[i].b].pos;
        const float r2 = nodes[edges[i].b].r;
        
        const Vec2 v12 = c2 - c1;
        const float dist = SFUTIL::length(v12);
        const Vec2 v12u = v12/dist;//u for unit vector

        const float cosTheta = (r1 - r2) / dist;
        const float sinTheta = sqrtf(1 - cosTheta * cosTheta);
        
        Vec2 tn_ccw(v12u.x * cosTheta - v12u.y * sinTheta,
                    v12u.x * sinTheta + v12u.y * cosTheta);
        
        Vec2  tn_cw(v12u.x * cosTheta + v12u.y * sinTheta,
                   -v12u.x * sinTheta + v12u.y * cosTheta);

        tangentPoints[i].p[2] = c1 + (tn_ccw * r1);//a
        tangentPoints[i].p[3] = c2 + (tn_ccw * r2);//b
        tangentPoints[i].p[0] = c1 + ( tn_cw * r1);//a
        tangentPoints[i].p[1] = c2 + ( tn_cw * r2);//b
    }

    return tangentPoints;
}

//another gem discovered somewhere lost in the abyss of time
//googling found some explanation
//https://stackoverflow.com/questions/16542042/fastest-way-to-sort-vectors-by-angle-without-actually-computing-that-angle
float pseudoAngle(const Vec2 &v)
{
    float p = v.x / (fabsf(v.x) + fabsf(v.y));
    return (v.y > 0.f ? 3.f - p : 1.f + p);//0-4
}

//used to sort neighbours in graph
struct AngleSort
{
    AngleSort(const std::vector<Node2D> &nodes_, const NODEID i):nodes(nodes_), pos(nodes[i].pos){}
    const std::vector<Node2D> &nodes;
    const Point pos;
    bool operator() (const std::pair<NODEID,EDGEID> &a, const std::pair<NODEID,EDGEID> &b) //(const NODEID a, const NODEID b)
    {
        const Vec2 A = nodes[a.first].pos - pos;
        const Vec2 B = nodes[b.first].pos - pos;
        
        return pseudoAngle(A) < pseudoAngle(B);
    }
};

void sortGraphCCW(std::vector<GraphNode> &graph, const std::vector<Node2D> &nodes)
{
    for(NODEID i = 0; i < graph.size(); ++i)
    {
        if(graph[i].neighbours.size()>2)
            std::sort(graph[i].neighbours.begin(), graph[i].neighbours.end(), AngleSort(nodes, i));
    }
}

NODEID nextNode(const std::vector<GraphNode> &graph, NODEID curId, NODEID lastId)
{
    const GraphNode &curNode = graph[curId];
    for(unsigned int i = 0; i < curNode.neighbours.size(); ++i)
    {
        if(curNode.neighbours[i].first == lastId)
            return curNode.neighbours[next(i, curNode.neighbours.size())].first;
    }

    return INVALID_ID;
}

}//empty namespace

OutlineGenerator::OutlineGenerator(const std::vector<Node2D> &_nodes, const std::vector<Edge> &_edges)
                : nodes(_nodes), edges(_edges), lastId(0), m_numVisits(0), m_totalNumVisits(0), isDone(false)
{
    graph = genGraph(nodes.size(), edges);
    sortGraphCCW(graph, nodes);
    curId = graph[0].neighbours[0].first;
    for(unsigned int i = 0; i < graph.size(); ++i)
        m_totalNumVisits += graph[i].neighbours.size();
    tangentPoints = calcTangentPoints(edges, nodes);
}

void OutlineGenerator::step()
{
    if(isDone = (m_numVisits == m_totalNumVisits))
        return;
    
    ++m_numVisits;
    ID nextId = addPoint();
    lastId = curId;
    curId = nextId;
}

void OutlineGenerator::generate()
{
    while(m_numVisits < m_totalNumVisits)
    {
        ++m_numVisits;
        ID nextId = addPoint();
        lastId = curId;
        curId = nextId;
    }
}

//uses curId, lastId, graph, nodes, edges, tangentPoints
NODEID OutlineGenerator::addPoint()
{
    //a b c (at b)
    NODEID nextId = nextNode(graph, curId, lastId);
    //use last cur and next to find the tangent line points
    const Node2D &curNode = nodes[curId];

    EDGEID lastEdgeId = INVALID_ID;
    EDGEID nextEdgeId = INVALID_ID;
    for(const auto &pair : graph[curId].neighbours)
    {
        if(pair.first == lastId)
            lastEdgeId = pair.second;
        if(pair.first == nextId)
            nextEdgeId = pair.second;
    }

    //assumes no singular graphs of 1 node alone
    bool isEndNode = graph[curId].neighbours.size() == 1;
    bool isFirstLastEdge = edges[lastEdgeId].a == curId;
    bool isFirstNextEdge = edges[nextEdgeId].a == curId;

//std::cout<<"At node "<<curId<<" ";
//if(isFirstLastEdge) std::cout<<" firstLast ";
//if(isFirstNextEdge) std::cout<<" firstNext ";
//std::cout<<std::endl;
//want the other side
    Point p1,p2,p3,p4;//OH MY WORD WHY DO I DO THIS TO MYSELF BECAUSE I DON"T WANT TO CALCULATE THE TANGENT TWICE
    if(isFirstLastEdge)//b-a edge but want flip rev indice
    {
        p1 = tangentPoints[lastEdgeId].p[3];
        p2 = tangentPoints[lastEdgeId].p[2];
    }
    else//a-b edge want a-b
    {
        p1 = tangentPoints[lastEdgeId].p[0];
        p2 = tangentPoints[lastEdgeId].p[1];
    }
    if(isFirstNextEdge)//b-c edge want b-c
    {
        p3 = tangentPoints[nextEdgeId].p[0];//01 or 23
        p4 = tangentPoints[nextEdgeId].p[1];
    }
    else//c-b edge want b-c
    {
        p3 = tangentPoints[nextEdgeId].p[3];
        p4 = tangentPoints[nextEdgeId].p[2];
    }

    if(isEndNode)
    {
        points.emplace_back(p2);
        const Node2D &lastNode = nodes[lastId];
        Vec2 v = SFUTIL::getUnitVec(curNode.pos - lastNode.pos);
        points.emplace_back(curNode.pos + v * curNode.r);
        points.emplace_back(p3);
    }
    else
    {
        Point intPt;
        if(!SFUTIL::lineSegsIntersect(p1,p2,p3,p4,&intPt))
        {
            points.emplace_back(p2);
            points.emplace_back(p3);
        }
        else
            points.emplace_back(intPt);
    }

    return nextId;
}