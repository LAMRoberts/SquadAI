#include "Level.h"

Level::Level(DirectX::XMINT2 mapSizes)
{
	// assign empty nodes to map vector
	Node node;
	map.assign((mapSizes.x * mapSizes.y), node);

	rowCount = mapSizes.y;
	columnCount = mapSizes.x;

	// loop each row
	int x = 0;
	for (int row = 0; row < mapSizes.y; row++)
	{
		// loop each col
		for (int col = 0; col < mapSizes.x; col++)
		{
			// initiale each node in map
			map[x].position = { row, col };
			map[x].isTraversable = true;

			map[x].parentPosition = { NULL, NULL };
			map[x].gCost = NULL;
			map[x].hCost = NULL;
			map[x].fCost = NULL;
		}
	}
}

void Level::setTraversable(DirectX::XMINT2 position)
{
	int pos = (columnCount * position.y) + position.x;

	map[pos].isTraversable = false;
}

void Level::findRoute(DirectX::XMINT2 startPos, DirectX::XMINT2 goalPos)
{
	// set positions
	startPosition = startPos;
	goalPosition = goalPos;

	// set current to start position
	DirectX::XMINT2 current = startPos;

	// add start node to closed list
	closed.push_back(current);

	// get neighbours of current node
	neighbours.clear();
	neighbours = findNeighbours(current);

	// add traversable neighbours to open list
	for (int i = 0; i < neighbours.size(); i++)
	{
		open.push_back(neighbours[i]);
	}

	while (!completeRoute)
	{
		// get lowest f cost in open list
		DirectX::XMINT2 current = findLowestFCost();

		// remove current from open list
		for (int i = 0; i < open.size(); i++)
		{
			open.erase(open.begin() + nodePosToArrayPos(current));
		}	

		// add current to closed list
		closed.push_back(current);

		// get neighbours of current node
		neighbours.clear();
		neighbours = findNeighbours(current);

		// for each neighbour in current
		for (int i = 0; i < neighbours.size(); i++)
		{
			// if neighbour is not in closed
			if (inClosedList(neighbours[i]) == -1)
			{
				// if neighbour is not in open list
				if (inOpenList(neighbours[i]) == -1)
				{
					// add to open list

					// find costs

				}
				else // neighbour is in open list
				{
					// TODO: if fCost is lower when we use the current genereated path
					if (true)
					{
						// update costs

						// update parent too
					}
				}
			}
		}



		// if current is goal then we have found our route
		if (current.x == goalPosition.x && current.y == goalPosition.y)
		{
			completeRoute = true;
		}
	}

	// now go backwards to trace our path
}

// returns a vector of valid, traversable neighbours
std::vector<DirectX::XMINT2> Level::findNeighbours(DirectX::XMINT2 current)
{
	std::vector<DirectX::XMINT2> results;

	// for each neighbour
	for (int i = 0; i < localPositions.size(); i++)
	{
		DirectX::XMINT2 neighbour = { current.x + localPositions[i].x,  current.y + localPositions[i].y };

		// if neighbour is a valid position
		if (neighbour.x >= 0	&& neighbour.y >= 0 && neighbour.x < columnCount	&& neighbour.y >= rowCount)
		{
			// if map position is traversable
			if (map[nodePosToArrayPos(neighbour)].isTraversable)
			{
				// set parent to current
				map[nodePosToArrayPos(current)].parentPosition = current;

				// set costs
				setCosts(neighbour);

				// add to vector
				results.push_back(neighbour);
			}
		}
	}

	return results;
}

// sets map node costs
void Level::setCosts(DirectX::XMINT2 nodePosition)
{
	// get array positions ffor node and parent
	int arrayPos = nodePosToArrayPos(nodePosition);
	int parentArrayPos = nodePosToArrayPos(map[arrayPos].parentPosition);

#pragma region G Cost

	int cost = 0;

	// check position relative to parent
	if (map[arrayPos].position.x != map[parentArrayPos].position.x)
	{
		cost++;
	}
	if (map[arrayPos].position.y != map[parentArrayPos].position.y)
	{
		cost++;
	}

	// set gCost (parents gCost + 10 or 14)
	switch (cost)
	{
	case 1:
	{
		map[arrayPos].gCost = map[parentArrayPos].gCost + 10;

		break;
	}
	case 2:
	{
		map[arrayPos].gCost = map[parentArrayPos].gCost + 14;

		break;
	}
	default:
	{
		break;
	}
	}

#pragma endregion
	
#pragma region H Cost

	int xDistance = 0;
	int yDistance = 0;

	// get x and y int distances to goal
	if (map[arrayPos].position.x <= goalPosition.x)
	{
		xDistance = goalPosition.x - nodePosition.x;
	}
	else
	{
		xDistance = nodePosition.x - goalPosition.x;
	}

	if (map[arrayPos].position.y <= goalPosition.y)
	{
		yDistance = goalPosition.y - nodePosition.y;
	}
	else
	{
		yDistance = nodePosition.y - goalPosition.y;
	}

	// set h cost based on manhattan distance
	if (xDistance > yDistance)
	{
		map[arrayPos].hCost = (14 * yDistance) + (10 * (xDistance - yDistance));
	}
	else
	{
		map[arrayPos].hCost = (14 * xDistance) + (10 * (yDistance - xDistance));
	}

#pragma endregion

#pragma region F Cost

	map[arrayPos].fCost = map[arrayPos].gCost + map[arrayPos].hCost;

#pragma endregion
}

// returns the map position of the lowest fCost node in open list
DirectX::XMINT2 Level::findLowestFCost()
{
	DirectX::XMINT2 result;
	int fCost = INT16_MAX;

	// for each node in open
	for (int i = 0; i < open.size(); i++)
	{
		// if nodes cost is lowest so far
		if (map[nodePosToArrayPos(open[i])].fCost < fCost)
		{
			// set this node to return result
			result = open[i];

			// update cost to compare nodes to
			fCost = map[nodePosToArrayPos(open[i])].fCost;
		}
	}

	return result;
}

// return position in closed list or -1 if not
int Level::inClosedList(DirectX::XMINT2 node)
{
	for (int i = 0; i < closed.size(); i++)
	{
		if (closed[i].x == node.x && closed[i].y == node.y)
		{
			return i;
		}
	}

	return -1;
}

// return position in open list or -1 if not
int Level::inOpenList(DirectX::XMINT2 node)
{
	for (int i = 0; i < open.size(); i++)
	{
		if (open[i].x == node.x && open[i].y == node.y)
		{
			return i;
		}
	}

	return -1;
}

// returns true if map node is traversable
bool Level::isTraversable(DirectX::XMINT2 node)
{
	if (map[nodePosToArrayPos(node)].isTraversable)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// return the map array position 
int Level::nodePosToArrayPos(DirectX::XMINT2 nodePos)
{
	return (columnCount * nodePos.y) + nodePos.x;
}