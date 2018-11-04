#pragma once

#include <DirectXMath.h>
#include <vector>

struct Node
{
	DirectX::XMINT2 position;
	bool isTraversable;

	DirectX::XMINT2 parentPosition;
	int gCost;	// how far from parent
	int hCost;	// how far from end node
	int fCost;	// gcost + hcost
};

class Level
{
public:
	// map methods
	Level(DirectX::XMINT2 mapSizes);

	void setTraversable(DirectX::XMINT2 position);

	// pathfinding methods
	void findRoute(DirectX::XMINT2 startPos, DirectX::XMINT2 goalPos);

	std::vector<DirectX::XMINT2> findNeighbours(DirectX::XMINT2 current);

	void setCosts(DirectX::XMINT2 node);

	DirectX::XMINT2 findLowestFCost();

	int inClosedList(DirectX::XMINT2 node);
	int inOpenList(DirectX::XMINT2 node);

	bool isTraversable(DirectX::XMINT2 node);

	int nodePosToArrayPos(DirectX::XMINT2 node);

private:

	// map members

	std::vector<Node> map;			// this is our game map
	int rowCount = 0;
	int columnCount = 0;

	// pathfinding members

	bool completeRoute = false;		// true if we have a*-ed to our goal

	std::vector<DirectX::XMINT2> open;			// list of nodes to evaluate
	std::vector<DirectX::XMINT2> closed;		// list of nodes evaluated
	std::vector<DirectX::XMINT2> neighbours;	// list of nodes nearby

	DirectX::XMINT2 startPosition;
	DirectX::XMINT2 goalPosition;

	const std::vector<DirectX::XMINT2> localPositions = 
	{
		{ -1, -1 }, { 0, -1 }, { 1, -1 },
		{ -1,  0 },            { 1,  0 },
		{ -1,  1 }, { 0,  1 }, { 1,  1 },
	};
};