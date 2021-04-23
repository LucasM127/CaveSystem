//https://www.hackerearth.com/practice/algorithms/graphs/minimum-spanning-tree/tutorial/
#include "Kruskal.hpp"

#include <numeric>

//HACK
namespace
{

std::vector<EDGEID> skippedEdgeIds;

}

KruskalMST::KruskalMST(const std::vector<Node2D> &inputNodes) : nodes(inputNodes)
{
    createEdges();
}

void KruskalMST::createEdges()
{
    for(size_t i = 0; i < nodes.size(); ++i)
        for(size_t j = i+1; j < nodes.size(); ++j)
        {
            edges.emplace_back(i,j,nodes);
        }
    auto edgeSort = [](const Edge &a, const Edge &b) -> bool
    {
        return a.w < b.w;
    };
    std::sort(edges.begin(), edges.end(), edgeSort);

    edgeIds.resize(edges.size());
    std::iota(edgeIds.begin(), edgeIds.end(), 0);

    visited.resize(nodes.size(), false);
    numVisited = 0;
    curEdgeId = 0;
}

//Find root of graph all joining become the same id ?
ID KruskalMST::root(ID x)
{
    while(edgeIds[x] != x)
    {
        edgeIds[x] = edgeIds[edgeIds[x]];
        x = edgeIds[x];
    }
    return x;
}

//connect the edges to each other union1 in tutorial
void KruskalMST::join(ID x, ID y)
{
    ID p = root(x);
    ID q = root(y);
    edgeIds[p] = edgeIds[q];
}

bool KruskalMST::isDone()
{
    if(chosenEdges.size() != nodes.size()-1)
        return false;
    for(unsigned int i = 1; i < chosenEdges.size(); ++i)
        if(edgeIds[i] != edgeIds[0])
            return false;
    return true;
}

//FIX THE EXIT CONDITION
//Done when root is same for all... 0-nodes.size()-1
void KruskalMST::genMST()
{
    for(size_t i = 0; i < edges.size(); ++i)
    {
        //Show the edge considering
        if(root(edges[i].a) != root(edges[i].b))
        {
            join(edges[i].a,edges[i].b);
            if(!visited[edges[i].a])
            {
                visited[edges[i].a] = true;
                ++numVisited;
            }
            if(!visited[edges[i].b])
            {
                visited[edges[i].b] = true;
                ++numVisited;
            }
            chosenEdges.emplace_back(edges[i]);
            //show closeEdges
            if(isDone())
                break;
        }
        //HACK
        else
            skippedEdgeIds.emplace_back(i);
    }
    //chosenEdges.insert(chosenEdges.end(),skippedEdgeIds.begin(),skippedEdgeIds.end());
    //for(EDGEID i : skippedEdgeIds)
      //  chosenEdges.emplace_back(edges[skippedEdgeIds[i]]);
      //  chosenEdges.emplace_back(edges[skippedEdgeIds[i]]);//add loops ???
}

bool KruskalMST::stepMST()
{
    if(isDone())
    {
        static bool T = true;
        if(T)
        for(EDGEID i : skippedEdgeIds)
            chosenEdges.emplace_back(edges[skippedEdgeIds[i]]);
        if(T)
        {
            T = false;
            return true;
        }
        return false;
    }
    if(curEdgeId < edges.size())
    {
        ID i = curEdgeId;
        if(root(edges[i].a) != root(edges[i].b))
        {
            join(edges[i].a,edges[i].b);
            if(!visited[edges[i].a])
            {
                visited[edges[i].a] = true;
                ++numVisited;
            }
            if(!visited[edges[i].b])
            {
                visited[edges[i].b] = true;
                ++numVisited;
            }
            chosenEdges.emplace_back(edges[i]);
        }
    }
    else
            skippedEdgeIds.emplace_back(curEdgeId);
    ++curEdgeId;
    return true;
}