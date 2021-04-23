#include "OutlineGraph.hpp"
#include <algorithm>
#include "SFGeometricFuncs.hpp"

namespace
{
//another gem discovered somewhere lost in the abyss of time
//googling found some explanation
//https://stackoverflow.com/questions/16542042/fastest-way-to-sort-vectors-by-angle-without-actually-computing-that-angle
float pseudoAngle(Vec2 &v)
{
    float p = v.x / (fabsf(v.x) + fabsf(v.y));
    return (v.y > 0.f ? 3.f - p : 1.f + p);//0-4
}

//used to sort neighbours in graph
struct AngleSort
{
    AngleSort(const std::vector<Node2D> &nodes_, const ID i):nodes(nodes_), base(nodes[i].pos){}
    const std::vector<Node2D> &nodes;
    const Point base;
    bool operator()(const ID a, const ID b)
    {
        Point A = nodes[a].pos - base;
        Point B = nodes[b].pos - base;
        
        return pseudoAngle(A) < pseudoAngle(B);
    }
};

ID next(ID i, ID sz)
{
    return i == sz - 1 ? 0 : i + 1;
}

void sortGraphCCW(std::vector<GraphNode> &graph, const std::vector<Node2D> &nodes)
{
    for(ID i = 0; i < graph.size(); ++i)
    {
        if(graph[i].neighbours.size()>2)
            std::sort(graph[i].neighbours.begin(), graph[i].neighbours.end(), AngleSort(nodes, i));
    }
}

ID nextNode(const std::vector<GraphNode> &graph, ID curId, ID lastId)
{
    const GraphNode &curNode = graph[curId];
    for(unsigned int i = 0; i < curNode.neighbours.size(); ++i)
    {
        if(curNode.neighbours[i] == lastId)
            return curNode.neighbours[next(i, curNode.neighbours.size())];
    }

    return -1;
}

//TODO: Fix float point error.  Adding two points when goes right (external) but should I if really small?????
ID addPoint(std::vector<Point> &points, ID curId, ID lastId, const std::vector<GraphNode> &graph, const std::vector<Node2D> &nodes)
{
    ID nextId = nextNode(graph, curId, lastId);
    const Node2D &curNode = nodes[curId];//then I won't need the node2d here.  just the edge ID...
    const Node2D &lastNode = nodes[lastId];
    const Node2D &nextNode = nodes[nextId];

    //Add a point or points to line at current node
    //3 Cases Internal Bend, External Bend, Cap Node
    //0 last, 1 current, 2 next
    Vec2 v01 = lastNode.pos - curNode.pos;
    Vec2 v01n = SFUTIL::getUnitVec<float>({-v01.y, v01.x});//n normal vector (orthoganol)
    Vec2 v12 = curNode.pos - nextNode.pos;
    Vec2 v12n = SFUTIL::getUnitVec<float>({-v12.y, v12.x});//n normal vector (orthoganol)
    bool toLeft = SFUTIL::cross<float>(v01,v12) > 0.f;

    if(graph[curId].neighbours.size() ==1)//Cap Node - add 3 points, one on left, right, and middle
    {
        points.emplace_back(curNode.pos + v01n * curNode.r);
        
        //and one in the middle...
        Vec2 v12unit = SFUTIL::getUnitVec<float>(v12);
        points.emplace_back(curNode.pos + v12unit * curNode.r);

        points.emplace_back(curNode.pos + v12n * curNode.r);
    }
    else if(toLeft)//External Bend - add 2 points (if not parallel that is)
    {
        points.emplace_back(curNode.pos + v01n * curNode.r);
        if(v01n != v12n)
            points.emplace_back(curNode.pos + v12n * curNode.r);
    }
    else//Interior Bend - add 1 point where the 2 lines intersect (miter point)
    {
        Point l01a = lastNode.pos + v01n * lastNode.r;
        Point l01b = curNode.pos + v01n * curNode.r;//outline line from last node to curNode

        Point l12a = curNode.pos + v12n * curNode.r;
        Point l12b = nextNode.pos + v12n * nextNode.r;//outline line from curNode to next node
        Point intPt;
        if(!SFUTIL::lineSegsIntersect<float>(l01a,l01b,l12a,l12b,&intPt))
        {//the intersection is at the endpoints within floaty point error so just use an end point
            points.push_back(l01b);
        }
        else points.push_back(intPt);
    }

    return nextId;
}

} //empty namespace

//do it all at once
std::vector<Point> genOutline(const std::vector<Node2D> &nodes, std::vector<GraphNode> &graph)
{
    sortGraphCCW(graph, nodes);
    ID curId = graph[0].neighbours[0];
    ID lastId = 0;
    unsigned int totalNumVisits = 0;
    for(unsigned int i = 0; i < graph.size(); ++i)
    {
        totalNumVisits += graph[i].neighbours.size();
    }

    std::vector<Point> points;
    unsigned int numVisits = 0;
    while (numVisits < totalNumVisits)
    {
        ++numVisits;
        ID nextId = addPoint(points, curId, lastId, graph, nodes);
        lastId = curId;
        curId = nextId;
    }
    
    return points;
}

//or step through for debugging/visualization
GenOutlineStepper::GenOutlineStepper(const std::vector<Node2D> &_nodes, std::vector<GraphNode> &_graph)
                : nodes(_nodes), graph(_graph), lastId(0), m_numVisits(0), m_totalNumVisits(0)
{
    isDone = false;
    sortGraphCCW(graph, nodes);
    curId = graph[0].neighbours[0];
    for(unsigned int i = 0; i < graph.size(); ++i)
    {
        m_totalNumVisits += graph[i].neighbours.size();
    }
}

void GenOutlineStepper::step()
{
    if(m_numVisits == m_totalNumVisits)
    {
        isDone = true;
        return;
    }
    ++m_numVisits;
    ID nextId = addPoint(points, curId, lastId, graph, nodes);
    lastId = curId;
    curId = nextId;
}