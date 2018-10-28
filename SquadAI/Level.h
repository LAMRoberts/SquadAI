#pragma once

#include <DirectXMath.h>
#include <vector>

struct Node
{
	DirectX::XMINT2 position;
	bool traversable;

	DirectX::XMINT2 parentPosition;
	int gCost;
	int hCost;
	int fCost;

};

class Level
{
public:
	// map methods
	Level(DirectX::XMINT2 mapSizes);

	// pathfinding methods

	Node findCellInMap(DirectX::XMINT2 position);

	void findRoute(DirectX::XMINT2 startPos, DirectX::XMINT2 goalPos);

	Node findLowestFCost();

	int inClosedList(Node node);
	int inOpenList(Node node);

	bool isTraversable(Node node);

private:

	// map members

	std::vector<Node> map;			// this is our game map


	// pathfinding members

	bool completeRoute = false;		// true if we have a*ed to our goal

	std::vector<Node> open;			// list of nodes to evaluate
	std::vector<Node> closed;		// list of nodes evaluated
};