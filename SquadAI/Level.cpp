#include "Level.h"

Level::Level(DirectX::XMINT2 mapSizes)
{
	// assign empty nodes to map vector
	Node node;
	map.assign((mapSizes.x * mapSizes.y), node);

	// loop each row
	int x = 0;
	for (int row = 0; row < mapSizes.x; row++)
	{
		// loop each col
		for (int col = 0; col < mapSizes.y; col++)
		{
			// initiale each node in map
			map[x].position = { row, col };
			map[x].traversable = true;

			map[x].parentPosition = { NULL, NULL };
			map[x].gCost = NULL;
			map[x].hCost = NULL;
			map[x].fCost = NULL;
		}
	}
}

Node Level::findCellInMap(DirectX::XMINT2 position)
{
	for (int i = 0; i < map.size(); i++)
	{
		if (position.x == map[i].position.x && position.x == map[i].position.x)
		{
			return map[i];
		}
	}

	return map[0];
}

void Level::findRoute(DirectX::XMINT2 startPos, DirectX::XMINT2 goalPos)
{
	Node startNode;
	startNode.position = findCellInMap(startPos).position;
	startNode.parentPosition = { NULL, NULL };
	startNode.fCost = NULL;

	open.push_back(startNode);

	while (!completeRoute)
	{
		// temp for the node we are evaluating
		Node current = findLowestFCost();

		//remove from open list and add to closed list
		open.erase(open.begin() + inOpenList(current));
		closed.push_back(current);

		// check that this node isnt the goal
		if (current.position.x == goalPos.x && current.position.y == goalPos.y)
		{
			completeRoute = true;
		}
		else
		{
			std::vector<Node> neighbours;

			for (int i = 0; i < neighbours.size(); i++)
			{
				if (isTraversable(neighbours[i]) || !inClosedList(neighbours[i]))
				{
					if (/*(new path to neighbour is shorter)*/ false || inOpenList(neighbours[i]))
					{
						neighbours[i].fCost = NULL;
						neighbours[i].parentPosition = current.position;

						if (!inOpenList(neighbours[i]))
						{
							open.push_back(neighbours[i]);
						}
					}
				}
			}
		}
	}

	// now we have the target node as our last entry in the neighbours list
	// we also have a parent for our target node, and each next parent save in neighbours list
	// we just have to 
}

Node Level::findLowestFCost()
{
	Node result = open[0];

	for (int i = 0; i < open.size(); i++)
	{
		if (open[i].fCost < result.fCost)
		{
			result = open[i];
		}
	}

	return result;
}

int Level::inClosedList(Node node)
{
	for (int i = 0; i < closed.size(); i++)
	{
		if (closed[i].position.x == node.position.x && closed[i].position.y == node.position.y)
		{
			return i;
		}
	}

	return -1;
}
int Level::inOpenList(Node node)
{
	for (int i = 0; i < open.size(); i++)
	{
		if (open[i].position.x == node.position.x && open[i].position.y == node.position.y)
		{
			return i;
		}
	}

	return -1;
}

bool Level::isTraversable(Node node)
{
	for (int i = 0; i < map.size(); i++)
	{
		if (node.position.x == map[i].position.x && node.position.y == map[i].position.y)
		{
			if (map[i].traversable)
			{
				return true;
			}
		}
	}

	return false;
}
