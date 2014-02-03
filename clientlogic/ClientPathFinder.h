#pragma once
#include <utility>
#include <map>
#include <vector>

typedef std::pair<int, int> PosNode;

class ClientPathFinder
{
public:

	ClientPathFinder() {}
	virtual ~ClientPathFinder() {}

	// x, y 좌표로 이동 가능한지 체크
	// 이동 가능하면 x, 불가능하면 y
	bool CanWalkBlock(const PosNode node);

	// start에서 end로 가기위한 경로 중
	// curX, curY에서 바로 다음 걸음을 가기 위한 노드 정보를 nextX, nextY로 지정
	// 경로가 있을 경우 true, 없을경우 false
	bool NextStep(const PosNode start, const PosNode dest, PosNode& nextNode);


private:

	// start에서 dest로의 경로를 찾음
	// 경로가 있을 경우 true, 없을경우 false
	bool FindPath(const PosNode start, const PosNode dest);

	inline int ManhattanDistance(const PosNode pos1, const PosNode pos2) { return abs(pos1.first - pos2.first) + abs(pos1.second - pos2.second); }

	struct AStarNode
	{
		PosNode node;
		int costHere;
		int costToGo;
		int nodeId;

		AStarNode(PosNode node, int costHere, int costToGo, int nodeId)
			: node(node), costHere(costHere), costToGo(costToGo), nodeId(nodeId)
		{
		}
	};

	struct AStarNodeComparator
	{
		bool operator() (const AStarNode& lhs, const AStarNode& rhs)
		{
			if(lhs.costHere + lhs.costToGo > rhs.costHere + rhs.costToGo)
				return true;
			
			if(lhs.costHere + lhs.costToGo == rhs.costHere + rhs.costToGo)
				return lhs.nodeId < rhs.nodeId;

			return false;
		}
	};
	
	typedef std::pair<PosNode, PosNode>		PathKey;
	typedef std::vector<PosNode>			PathList;
	typedef	std::map<PathKey, PosNode>		PathNextPosMap;

	PathNextPosMap	pathNextNodeCacheMap;
};

extern ClientPathFinder* GClientPathFinder;