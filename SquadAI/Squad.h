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
	void setID(UINT ID);
	void setUnitIDs();
	UINT getSquadID();

	void setPath(std::vector<DirectX::XMINT2> AStarredPath);
	void resetPath();
	std::vector<DirectX::XMINT2> getPath();

	int nextNode = 0;
	DirectX::XMFLOAT3 squadPosition = { 0.0f, 0.0f, 0.0f };

private:

	int squadID;

	UINT squadSize = 9;

	DirectX::XMMATRIX world;

	DirectX::XMMATRIX oTranslation;

	Formation formation = Formation::SQUARE;
	int formNo = 0;	// number of rows / columns if not square

	std::vector<DirectX::XMINT2> path;
};