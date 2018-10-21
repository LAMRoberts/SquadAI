#pragma once
#include <vector>

#include "CubeObject.h"

enum class Formation
{
	SQUARE	= 0,
	COLUMNS = 1,
	ROWS	= 2,
};

class Squad
{
public:
	Squad(Renderer & renderer, UINT size, Formation formation, int formNo);
	~Squad();

	std::vector<CubeObject> cubeObjs;

	UINT getSquadSize();
	Formation getFormation();
	int getFormationNumber();

private:

	UINT squadSize = 9;

	DirectX::XMFLOAT3 pos;
	DirectX::XMMATRIX world;

	DirectX::XMMATRIX oTranslation;

	Formation formation = Formation::SQUARE;
	int formNo = 0;	// number of rows / columns if not square
};