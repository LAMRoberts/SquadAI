#include "Squad.h"

Squad::Squad(Renderer & renderer, UINT size, Formation form, int number)
{
	squadSize = size;
	formation = form;
	formNo = number;

	CubeObject defaultCube(renderer);
	cubeObjs.assign(squadSize, defaultCube);

	// init cube pos's
	for (int i = 0; i < squadSize; i++)
	{
		switch (formation)
		{
		case Formation::SQUARE:
		{
			int s = sqrt(squadSize);
			int row = i / s;
			int col = i % s;

			cubeObjs[i].translate((2 * row), 0.0f, (2 * col));

			break;
		}
		case Formation::ROWS:
		{
			int row = i / formNo;
			int col = i % formNo;

			cubeObjs[i].translate((2 * row), 0.0f, (2 * col));

			break;
		}
		case Formation::COLUMNS:
		{
			int row = i / formNo;
			int col = i % formNo;

			cubeObjs[i].translate((2 * col), 0.0f, (2 * row));

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
