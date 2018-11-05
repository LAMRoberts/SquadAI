#include "Squad.h"

Squad::Squad(Renderer & renderer, UINT size, Formation form, int number)
{
	squadSize = size;
	formation = form;
	formNo = number;

	CubeObject defaultCube(renderer);
	cubeObjs.assign(squadSize, defaultCube);

	// init cube IDs and pos's
	for (int i = 0; i < squadSize; i++)
	{
		switch (formation)
		{
		case Formation::SQUARE:
		{
			int s = sqrt(squadSize);
			int row = i / s;
			int col = i % s;

			cubeObjs[i].translate((2.0f * row), 0.0f, (2.0f * col));

			break;
		}
		case Formation::ROWS:
		{
			int row = i / formNo;
			int col = i % formNo;

			cubeObjs[i].translate((2.0f * row), 0.0f, (2.0f * col));

			break;
		}
		case Formation::COLUMNS:
		{
			int row = i / formNo;
			int col = i % formNo;

			cubeObjs[i].translate((2.0f * col), 0.0f, (2.0f * row));

			break;
		}
		default:
		{
			break;
		}
		}
	}
}

Squad::~Squad()
{
}

UINT Squad::getSquadSize()
{
	return squadSize;
}
Formation Squad::getFormation()
{
	return formation;
}
int Squad::getFormationNumber()
{
	return formNo;
}

void Squad::setID(UINT ID)
{
	squadID = ID;
}
void Squad::setUnitIDs()
{
	for (int i = 0; i < squadSize; i++)
	{
		cubeObjs[i].setID(DirectX::XMINT2{ squadID, i });
	}
}
UINT Squad::getSquadID()
{
	return squadID;
}

void Squad::setPath(std::vector<DirectX::XMINT2> AStarredPath)
{
	path = AStarredPath;
}

void Squad::resetPath()
{
	path.clear();
}

std::vector<DirectX::XMINT2> Squad::getPath()
{
	return std::vector<DirectX::XMINT2>(path);
}
