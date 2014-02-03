#include "stdafx.h"
#include "ClientPathFinder.h"
#include "tilemap.h"

#include <set>
#include <queue>


bool ClientPathFinder::CanWalkBlock(const PosNode node)
{
    return cltm_query_obstacle(node.first, node.second) == 1 ? false : true;
}

bool ClientPathFinder::NextStep(const PosNode start, const PosNode dest, PosNode& nextNode)
{
    PathNextPosMap::iterator it = pathNextNodeCacheMap.find(std::make_pair(start, dest));
    if(it != pathNextNodeCacheMap.end())
    {
        nextNode = it->second;
        return true;
    }

    if(FindPath(start, dest))
    {
        PathNextPosMap::iterator it = pathNextNodeCacheMap.find(std::make_pair(start, dest));
        if(it == pathNextNodeCacheMap.end())
        {
            // 시스템 죽여버려야 함
            // 이런일은 결코 발생하지 않음
            return false;
        }

        nextNode = it->second;
        return true;
    }

    return false;
}

bool ClientPathFinder::FindPath(PosNode start, PosNode dest)
{
    using namespace std;

    const int dx[4] = {0, 0, 1, -1};
    const int dy[4] = {-1, 1, 0, 0};

    map<PosNode, PosNode>	nodeParentMap;
    map<PosNode, int>		nodeCostMap;
    set<PosNode>			openSet, closedSet;

    priority_queue<AStarNode, vector<AStarNode>, AStarNodeComparator> aStarQueue;

    // 시작지점 초기화
    int nodeId = 0;
    aStarQueue.push(AStarNode(start, 0, ManhattanDistance(start, dest), nodeId++));
    nodeParentMap[start] = start;
    nodeCostMap[start] = ManhattanDistance(start, dest);
    openSet.insert(start);
    while(aStarQueue.empty() == false)
    {
        AStarNode node = aStarQueue.top();
        aStarQueue.pop();
        if(nodeCostMap[node.node] < node.costHere + node.costToGo) continue;

        openSet.erase(openSet.find(node.node));
        closedSet.insert(node.node);

        // base case - Find dest!!!
        if(node.node == dest)
        {
            PosNode tempNode = node.node;
            while(nodeParentMap[tempNode] != tempNode)
            {
                pathNextNodeCacheMap[make_pair(nodeParentMap[tempNode], tempNode)] = tempNode;
                pathNextNodeCacheMap[make_pair(nodeParentMap[tempNode], dest)] = tempNode;
                tempNode = nodeParentMap[tempNode];
            }
            return true;
        }

        for(int i = 0; i < 4; i++)
        {
            int nextX = node.node.first + dx[i];
            int nextY = node.node.second + dy[i];
            PosNode nextNode(nextX, nextY);
            if(CanWalkBlock(nextNode) == false) continue;
            if(closedSet.find(nextNode) != closedSet.end()) continue;

            int nextCostG = node.costHere + 1;
            int nextCostH = ManhattanDistance(nextNode, dest);
            if(openSet.find(nextNode) != openSet.end() && nodeCostMap[nextNode] <= (nextCostG + nextCostH)) continue;

            aStarQueue.push(AStarNode(nextNode, nextCostG, nextCostH, nodeId++));
            nodeParentMap[nextNode] = node.node;
            nodeCostMap[nextNode] = nextCostG + nextCostH;
            openSet.insert(nextNode);
        }
    }

    return true;
}

ClientPathFinder* GClientPathFinder = nullptr;

