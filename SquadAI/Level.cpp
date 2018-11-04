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
	for (int row = 0; row < rowCount; row++)
	{
		// loop each col
		for (int col = 0; col < columnCount; col++)
		{
			// initiale each node in map
			map[x].position = { col, row };
			map[x].isTraversable = true;

			map[x].parentPosition = { NULL, NULL };
			map[x].gCost = INT16_MAX;
			map[x].hCost = INT16_MAX;
			map[x].fCost = INT16_MAX;

			x++;
		}
	}
}

void Level::setTraversable(DirectX::XMINT2 position)
{
	int pos = (columnCount * position.y) + position.x;

	map[pos].isTraversable = false;
}

std::vector<DirectX::XMINT2> Level::findRoute(DirectX::XMINT2 startPos, DirectX::XMINT2 goalPos)
{
	// cleanup previous route first
	cleanupRoute();

	// this is what we will be returning
	std::vector<DirectX::XMINT2> path;

	// set positions
	startPosition = startPos;
	goalPosition = goalPos;

	// set current to start position
	DirectX::XMINT2 current = startPos;

	// add start node to closed list
	closed.push_back(current);

	// set parent of start node (this is really bad but is the easiest way)
	map[nodePosToArrayPos(current)].parentPosition = current;

	// calculate costs for first node
	DirectX::XMINT3 c = setCosts(current);
	map[nodePosToArrayPos(current)].gCost = c.x;
	map[nodePosToArrayPos(current)].hCost = c.y;
	map[nodePosToArrayPos(current)].fCost = c.z;

	// get neighbours of first node
	neighbours = findNeighbours(current);

	// add traversable neighbours to open list
	for (int i = 0; i < neighbours.size(); i++)
	{
		// add to open
		open.push_back(neighbours[i]);
	}

	while (!completeRoute)
	{
		// get lowest f cost in open list
		DirectX::XMINT2 current = findLowestFCost();

		// remove current from open list
		for (int i = 0; i < open.size(); i++)
		{
			if (open[i].x == current.x && open[i].y == current.y)
			{
				open.erase(open.begin() + i);
			}
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
					open.push_back(neighbours[i]);
				}

				// find costs
				setCosts(neighbours[i]);
			}
		}

		// for each node in open
		for (int i = 0; i < open.size(); i++)
		{
			// if goal is in open
			if (open[i].x == goalPosition.x && open[i].y == goalPosition.y)
			{
				// add it to closed
				closed.push_back(open[i]);

				// remove it from open
				open.erase(open.begin() + i);

				// set route to complete
				completeRoute = true;
			}
		}
	}

	// add goal to path
	path.reserve(1);
	path.insert(path.begin(), startPosition);

	// set parent of added node
	DirectX::XMINT2 parent = map[nodePosToArrayPos(closed[closed.size() - 1])].position;

	// while parent is not start
	while (parent.x != startPosition.x || parent.y != startPosition.y)
	{
		// add parent to start of path
		path.insert(path.begin() + 1, parent);

		// update parent
		parent = map[nodePosToArrayPos(parent)].parentPosition;
	}

	return path;
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
		if (neighbour.x >= 0 && neighbour.y >= 0 && neighbour.x < columnCount && neighbour.y < rowCount)
		{
			// if map position is traversable
			if (map[nodePosToArrayPos(neighbour)].isTraversable)
			{
				// if no parent has been set
				if (map[nodePosToArrayPos(neighbour)].parentPosition.x == 0 && map[nodePosToArrayPos(neighbour)].parentPosition.y == 0)
				{
					// set parent to current
					map[nodePosToArrayPos(neighbour)].parentPosition = current;
				}

				// new fCost based on current route
				DirectX::XMINT3 c = setCosts(neighbour);
				
				// if new fCost (c) is less than previous fCost
				if (c.z < map[nodePosToArrayPos(neighbour)].fCost)
				{
					// set parent to current
					map[nodePosToArrayPos(neighbour)].parentPosition = current;

					// set costs to new path costs
					map[nodePosToArrayPos(neighbour)].gCost = c.x;
					map[nodePosToArrayPos(neighbour)].hCost = c.y;
					map[nodePosToArrayPos(neighbour)].fCost = c.z;
				}

				// add to vector
				results.push_back(neighbour);
			}
		}
	}

	return results;
}

// sets map node costs
DirectX::XMINT3 Level::setCosts(DirectX::XMINT2 nodePosition)
{
	// get array positions ffor node and parent
	int arrayPos = nodePosToArrayPos(nodePosition);
	int parentArrayPos = nodePosToArrayPos(map[arrayPos].parentPosition);

	DirectX::XMINT3 costs = { INT16_MAX, INT16_MAX, INT16_MAX };

	// check position relative to parent
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
			costs.x = map[parentArrayPos].gCost + 10;

			break;
		}
		case 2:
		{
			costs.x = map[parentArrayPos].gCost + 14;

			break;
		}
		default:
		{
			costs.x = 0;
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
			costs.y = (14 * yDistance) + (10 * (xDistance - yDistance));
		}
		else
		{
			costs.y = (14 * xDistance) + (10 * (yDistance - xDistance));
		}

#pragma endregion

#pragma region F Cost

		costs.z = costs.x + costs.y;

#pragma endregion

	return costs;
}

// returns the map position of the lowest fCost node in open list
DirectX::XMINT2 Level::findLowestFCost()
{
	DirectX::XMINT2 result = { NULL, NULL };
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

void Level::cleanupRoute()
{
	if (completeRoute == true)
	{
		completeRoute == false;
	}

	if (open.size() != 0)
	{
		open.clear();
	}

	if (closed.size() != 0)
	{
		closed.clear();
	}

	if (neighbours.size() != 0)
	{
		neighbours.clear();
	}

	if (startPosition.x != NULL && startPosition.y != NULL)
	{
		startPosition.x = NULL;
		startPosition.y = NULL;
	}

	if (goalPosition.x != NULL && goalPosition.y != NULL)
	{
		goalPosition.x = NULL;
		goalPosition.y = NULL;
	}
}