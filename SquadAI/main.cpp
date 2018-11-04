#include <Windows.h>
#include <stdlib.h>
#include <chrono>

#include "Window.h"
#include "Renderer.h"
#include "Cube.h"
#include "Squad.h"
#include "Level.h"

#pragma region Enums

enum Direction
{
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3,
	FORWARD = 4,
	BACK = 5,
};
enum Axis
{
	X = 0,
	Y = 1,
	Z = 2
};

#pragma endregion

#pragma region Gross Global Variables

// FPS deltaTime
float deltaTime = 0.0f;

// ewwww global members
int windowWidth = 0;
int windowHeight = 0;
bool highlightingUnit = false; // highlighting a squad or unit?
DirectX::XMINT2 selected = { 0, 0 }; // x = squad number, y = unit number

#pragma endregion

#pragma region Forward Declares

// forward declarations
void moveCamera(Renderer & renderer, std::vector<bool> direction);
bool isUnitSelected(DirectX::XMINT2 unitID);
void selectObject(POINT & mousePos, Window & window, Renderer & renderer, std::vector<Squad> & squads);
void pickRayVector(Renderer & renderer, float mouseX, float mouseY, DirectX::XMVECTOR & pickRayInWorldSpacePos, DirectX::XMVECTOR & pickRayInWorldSpaceDir);
bool pick(DirectX::XMVECTOR pickRayInWorldSpacePos, DirectX::XMVECTOR pickRayInWorldSpaceDir, std::vector<DirectX::XMFLOAT3>& vertPosArray, std::vector<DWORD>& indexPosArray, DirectX::XMMATRIX& worldSpace);
bool PointInTriangle(DirectX::XMVECTOR & triV1, DirectX::XMVECTOR & triV2, DirectX::XMVECTOR & triV3, DirectX::XMVECTOR & point);
DirectX::XMINT2 getfloorTile(POINT & mousePos, Window & window, Renderer & renderer, std::vector<CubeObject> & floor);
DirectX::XMFLOAT3 findDistance(DirectX::XMFLOAT3 currentPos, DirectX::XMINT2 nextGridPos);

#pragma endregion

// weird windows version of main
int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
#pragma region Pre-Loop Init

	//create window
	windowWidth = 800;
	windowHeight = 800;
	Window window(windowWidth, windowHeight);

	// get mouse position
	POINT mousePos;

	//create renderer and init D3D
	float clearColour[] = { 0.2f, 0.3f, 0.5f, 1.0f };
	Renderer renderer(window, clearColour);

	// create cube
	Cube cube(renderer);

	// finish init
	renderer.setViewport();
	renderer.createConstantBuffer();
	UINT indices = cube.getIndexCount();

	// create message
	MSG msg = { 0 };

	// cam Directions
	std::vector<bool> camDirs;
	camDirs.assign(6, false);

	// tests for matrices
	bool shifting = false;
	int clicking = 0;
	bool rotate = false;
	std::vector<float> traDirs;
	traDirs.assign(3, 0.0f);
	float traSpeed = 0.0f;
	bool scale = false;

	// init squad prefab with units in formation
	Squad squareSquad(renderer, 9, Formation::SQUARE, 0);
	Squad rowsSquad(renderer, 6, Formation::ROWS, 3);
	Squad columnsSquad(renderer, 8, Formation::COLUMNS, 4);

	// init 3 squads of 9 units
	std::vector<Squad> squads;
	squads.assign(1, squareSquad);
	squads.insert(squads.end(), rowsSquad);
	squads.insert(squads.end(), columnsSquad);

	float squadXOffset = 0.0f;
	float squadSeparation = 2.0f;

	// init level with map size
	DirectX::XMINT2 mapSize = { 20, 20 };
	Level level(mapSize);

	// init level cubes
	CubeObject cObject(renderer);
	std::vector<CubeObject> floor;
	floor.assign((mapSize.x * mapSize.y), cObject);

	//	set floor cube positions
	for (int row = 0; row < sqrt(floor.size()); row++)
	{
		for (int col = 0; col < sqrt(floor.size()); col++)
		{
			int i = (sqrt(floor.size()) * row) + col;
			floor[i].translate((float)col, -1.0f, (float)row);
		}
	}

	// set squad separation, ID numbers and map position
	for (int i = 0; i < squads.size(); i++)
	{
		// set squad offset
		int unitsInRow = 0;
		switch (squads[i].getFormation())
		{
		case Formation::SQUARE:
		{
			unitsInRow = sqrt(squads[i].getSquadSize());

			break;
		}
		case Formation::ROWS:
		{
			unitsInRow = squads[i].getSquadSize() / squads[i].getFormationNumber();

			int a = squads[i].getSquadSize() % squads[i].getFormationNumber();

			if (a != 0)
			{
				unitsInRow += 1;
			}

			break;
		}
		case Formation::COLUMNS:
		{
			unitsInRow = squads[i].getFormationNumber();

			break;
		}
		default:
		{
			break;
		}
		}

		// translate units and set map positions
		for (int j = 0; j < squads[i].getSquadSize(); j++)
		{
			// translate units
			squads[i].cubeObjs[j].translate(squadXOffset, 0.0f, 0.0f);

			// get unit position
			DirectX::XMFLOAT3 unitPos = squads[i].cubeObjs[j].position;

			// convert squad world position to grid position
			DirectX::XMINT2 unitPosition = { (int)unitPos.x, (int)unitPos.z };

			// set grid square to impassible
			//TODO: figure this shit out
			//level.setTraversable(unitPosition);
		}

		// update squadOffset
		squadXOffset += unitsInRow + (unitsInRow - 1) + squadSeparation;

		// set IDs
		squads[i].setID(i);
		squads[i].setUnitIDs();
	}

#pragma endregion

	// main loop
	while (msg.message != WM_QUIT)
	{
	#pragma region Init Loop

		// start delta time clock
		auto started = std::chrono::high_resolution_clock::now();

#pragma endregion
		
	#pragma region Inputs

		// check input
		if (msg.message == WM_KEYDOWN)
		{
			float speed = deltaTime * 0.00001f;

			switch (msg.wParam)
			{
			case 16: // left shift
			{
				shifting = true;
				highlightingUnit = true;

				break;
			}
			case 'a' - 32:
			{
				camDirs[LEFT] = true;

				break;
			}
			case 'd' - 32:
			{
				camDirs[RIGHT] = true;

				break;
			}
			case 'w' - 32:
			{
				camDirs[FORWARD] = true;

				break;
			}
			case 's' - 32:
			{
				camDirs[BACK] = true;

				break;
			}
			case 'e' - 32:
			{
				camDirs[UP] = true;

				break;
			}
			case 'q' - 32:
			{
				camDirs[DOWN] = true;

				break;
			}
			case 'r' - 32:
			{
				rotate = true;

				break;
			}
			case VK_LEFT:
			{
				traDirs[X] = -speed;

				break;
			}
			case VK_RIGHT:
			{
				traDirs[X] = speed;

				break;
			}
			case VK_UP:
			{
				traDirs[Z] = speed;

				break;
			}
			case VK_DOWN:
			{
				traDirs[Z] = -speed;

				break;
			}
			case 49: // 1
			{
				if (!shifting)
				{
					selected.x = 0;
				}
				else
				{
					selected.y = 0;
				}

				break;
			}
			case 50: // 2
			{
				if (!shifting)
				{
					selected.x = 1;
				}
				else
				{
					selected.y = 1;
				}

				break;
			}
			case 51: // 3
			{
				if (!shifting)
				{
					selected.x = 2;
				}
				else
				{
					selected.y = 2;
				}

				break;
			}
			case 52: // 4
			{
				if (!shifting)
				{
					selected.x = 3;
				}
				else
				{
					selected.y = 3;
				}

				break;
			}
			case 53: // 5
			{
				if (!shifting)
				{
					selected.x = 4;
				}
				else
				{
					selected.y = 4;
				}

				break;
			}
			case 54: // 6
			{
				if (!shifting)
				{
					selected.x = 5;
				}
				else
				{
					selected.y = 5;
				}

				break;
			}
			case 55: // 7
			{
				if (!shifting)
				{
					selected.x = 6;
				}
				else
				{
					selected.y = 6;
				}

				break;
			}
			case 56: // 8
			{
				if (!shifting)
				{
					selected.x = 7;
				}
				else
				{
					selected.y = 7;
				}

				break;
			}
			case 57: // 9
			{
				if (!shifting)
				{
					selected.x = 8;
				}
				else
				{
					selected.y = 8;
				}

				break;
			}
			default:
			{
				break;
			}
			}
		}
		else if (msg.message == WM_KEYUP)
		{
			switch (msg.wParam)
			{
			case 16:
			{
				shifting = false;
				highlightingUnit = false;

				break;
			}
			case 'a' - 32:
			{
				camDirs[LEFT] = false;

				break;
			}
			case 'd' - 32:
			{
				camDirs[RIGHT] = false;

				break;
			}
			case 'w' - 32:
			{
				camDirs[FORWARD] = false;

				break;
			}
			case 's' - 32:
			{
				camDirs[BACK] = false;

				break;
			}
			case 'e' - 32:
			{
				camDirs[UP] = false;

				break;
			}
			case 'q' - 32:
			{
				camDirs[DOWN] = false;

				break;
			}
			case 'r' - 32:
			{
				rotate = false;

				break;
			}
			case VK_LEFT:
			case VK_RIGHT:
			{
				traDirs[X] = 0.0f;

				break;
			}
			case VK_UP:
			case VK_DOWN:
			{
				traDirs[Z] = 0.0f;

				break;
			}
			default:
			{
				break;
			}
			}
		}
		else if (msg.message == WM_LBUTTONDOWN)
		{
			if (clicking == 0)
			{
				clicking++;

				selectObject(mousePos, window, renderer, squads);
			}
		}
		else if (msg.message == WM_LBUTTONUP)
		{
			if (clicking != 0)
			{
				clicking--;
			}
		}
		else if (msg.message == WM_RBUTTONDOWN)
		{
			if (clicking == 0)
			{
				clicking++;

				// set temp position of selected unit
				DirectX::XMINT2 squadPosition = { (int)squads[selected.x].cubeObjs[selected.y].position.x, (int)squads[selected.x].cubeObjs[selected.y].position.z };
				DirectX::XMINT2 floorPosition = getfloorTile(mousePos, window, renderer, floor);

				// if clicked on valid floor tile
				if (floorPosition.x != -1 && floorPosition.y != -1)
				{
					// set path based on unit position annd floor tile clicked on
					squads[selected.x].setPath(level.findRoute(squadPosition, floorPosition));
				}
			}
		}
		else if (msg.message == WM_RBUTTONUP)
		{
			if (clicking != 0)
			{
				clicking--;
			}
		}

		// deal with message
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	#pragma endregion
		
	#pragma region Start Update

		//start frame
		renderer.beginFrame();

#pragma endregion		

	#pragma region Main Update Loop

		// move camera based on direction keys pressed
		moveCamera(renderer, camDirs);

		// render floor
		for (int i = 0; i < floor.size(); i++)
		{
			floor[i].preUpdate(renderer);
			// thers nerthin here cuz they's not gerna moove anyhways
			floor[i].postUpdate(renderer);

			renderer.draw(indices);
		}

		// move and render squads
		for (int i = 0; i < squads.size(); i++)
		{
			int unitProcessing = 0;

			for (int j = 0; j < squads[i].getSquadSize(); j++)
			{
				squads[i].cubeObjs[j].preUpdate(renderer);

				// new pathfinding movement
				if (squads[i].getPath().size() != 0)
				{
					// set position to next node
					DirectX::XMFLOAT3 difference = findDistance(squads[i].cubeObjs[j].position, squads[i].getPath()[squads[i].nextNode]);
					
					// if translating last unit in squad
					if (unitProcessing == (squads[i].getSquadSize() - 1))
					{
						// update nextNode
						squads[i].nextNode++;
					}
					
					// translate unit to next node
					squads[i].cubeObjs[j].translate(difference.x, difference.y, difference.z);
				}

				// old key movement
				if (false)
				{
					// if specific unit is selected or entire unit is selected
					if ((isUnitSelected(DirectX::XMINT2{ i, j }) && highlightingUnit) || (i == selected.x && !highlightingUnit))
					{
						// translate
						squads[i].cubeObjs[j].translate(traDirs[X], traDirs[Y], traDirs[Z]);
					}
				}

				// rotation
				if (rotate && isUnitSelected(DirectX::XMINT2{ i, j }) && highlightingUnit)
				{
					squads[i].cubeObjs[j].rotate(0.0002f, 0.0f, 1.0f, 0.0f);
				}

				// scaling
				if (i != selected.x)
				{
					squads[i].cubeObjs[j].scale(1.0f, 1.0f, 1.0f);
				}
				else
				{
					if (j == selected.y && highlightingUnit)
					{
						squads[i].cubeObjs[j].scale(1.0f, 2.0f, 1.0f);
					}
					else
					{
						squads[i].cubeObjs[j].scale(1.0f, 1.5f, 1.0f);
					}
				}

				squads[i].cubeObjs[j].postUpdate(renderer);

				renderer.draw(indices);
			}
		}

#pragma endregion

	#pragma region Clean Up Loop

		// clean up frame
		renderer.endFrame();

		// set delta time
		auto done = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(done - started).count();

#pragma endregion
	}

	// exit without a problem
	return 0;
}

// function for camera movement
// TODO: add camera rotation
void moveCamera(Renderer & renderer, std::vector<bool> direction)
{
	float speed =  deltaTime * 0.00005f;

	if (direction[LEFT])
	{
		renderer.moveCamera(-speed, 0.0f, 0.0f);
	}
	if (direction[RIGHT])
	{
		renderer.moveCamera(speed, 0.0f, 0.0f);
	}
	if (direction[UP])
	{
		renderer.moveCamera(0.0f, speed, 0.0f);
	}
	if (direction[DOWN])
	{
		renderer.moveCamera(0.0f, -speed, 0.0f);
	}
	if (direction[FORWARD])
	{
		renderer.moveCamera(0.0f, 0.0f, speed);
	}
	if (direction[BACK])
	{
		renderer.moveCamera(0.0f, 0.0f, -speed);
	}
}

// return true if unit is highlighted
bool isUnitSelected(DirectX::XMINT2 unitID)
{
	if (unitID.x == selected.x && unitID.y == selected.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// functions for picking
void selectObject(POINT & mousePos, Window & window, Renderer & renderer, std::vector<Squad> & squads)
{
	GetCursorPos(&mousePos);
	ScreenToClient(window.getHandle(), &mousePos);

	DirectX::XMVECTOR rayOrigin = { 0 };
	DirectX::XMVECTOR rayDirection = { 0 };

	pickRayVector(renderer, mousePos.x, mousePos.y, rayOrigin, rayDirection);

	bool foundUnit = false;

	// for each squad
	for (int i = 0; i < squads.size(); i++)
	{
		// for each unit in each squad
		for (int j = 0; j < squads[i].cubeObjs.size(); j++)
		{
			// if clicked on
			if (pick(rayOrigin, rayDirection, squads[i].cubeObjs[j].getVertexPositions(), squads[i].cubeObjs[j].getIndices(), squads[i].cubeObjs[j].getWorldMatrix()))
			{
				// if a unit hasnt been selected this time
				if (foundUnit == false)
				{
					// set selected to this units ID
					selected = { i, j };

					foundUnit = true;
				}
			}
		}
	}

}
void pickRayVector(Renderer & renderer, float mouseX, float mouseY, DirectX::XMVECTOR & rayOrigin, DirectX::XMVECTOR & rayDirection)
{
	DirectX::XMVECTOR pRayOrigin = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR pRayDirection = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	
	float rayX = 0.0f;
	float rayY = 0.0f;
	float rayZ = 0.0f;

	// 2D screen position to 3D view position
	DirectX::XMVECTOR xv = renderer.camProjection.r[0];
	DirectX::XMVECTOR yv = renderer.camProjection.r[1];

	rayX = (((2.0f * mouseX) / windowWidth) - 1) / DirectX::XMVectorGetX(xv);
	rayY = -(((2.0f * mouseY) / windowHeight) - 1) / DirectX::XMVectorGetY(yv);
	rayZ = 1.0f;

	pRayDirection = DirectX::XMVectorSet(rayX, rayY, rayZ, 0.0f);

	DirectX::XMMATRIX rayWorldMatrix;
	DirectX::XMVECTOR defaultVector;    

	rayWorldMatrix = DirectX::XMMatrixInverse(&defaultVector, renderer.camView);

	rayOrigin = XMVector3TransformCoord(pRayOrigin, rayWorldMatrix);
	rayDirection = XMVector3TransformNormal(pRayDirection, rayWorldMatrix);
}
bool pick(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDirection, std::vector<DirectX::XMFLOAT3> & vertexArray, std::vector<DWORD> & indexArray, DirectX::XMMATRIX & worldMatrix)
{
	for (int i = 0; i < indexArray.size() / 3; i++)
	{
		DirectX::XMVECTOR vertex0 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR vertex1 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR vertex2 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		DirectX::XMFLOAT3 tempVertex0;
		DirectX::XMFLOAT3 tempVertex1;
		DirectX::XMFLOAT3 tempVertex2;

		// get triangle 
		tempVertex0 = vertexArray[indexArray[(i * 3) + 0]];
		tempVertex1 = vertexArray[indexArray[(i * 3) + 1]];
		tempVertex2 = vertexArray[indexArray[(i * 3) + 2]];

		vertex0 = DirectX::XMVectorSet(tempVertex0.x, tempVertex0.y, tempVertex0.z, 0.0f);
		vertex1 = DirectX::XMVectorSet(tempVertex1.x, tempVertex1.y, tempVertex1.z, 0.0f);
		vertex2 = DirectX::XMVectorSet(tempVertex2.x, tempVertex2.y, tempVertex2.z, 0.0f);

		// transform the vertices to world space
		vertex0 = DirectX::XMVector3TransformCoord(vertex0, worldMatrix);
		vertex1 = DirectX::XMVector3TransformCoord(vertex1, worldMatrix);
		vertex2 = DirectX::XMVector3TransformCoord(vertex2, worldMatrix);

		// find the normal using U and V
		DirectX::XMVECTOR U = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR V = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR faceNormal = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		U = DirectX::XMVectorSubtract(vertex1, vertex0);
		V = DirectX::XMVectorSubtract(vertex2, vertex0);

		// get face normal from U and V
		faceNormal = DirectX::XMVector3Cross(U, V);
		faceNormal = DirectX::XMVector3Normalize(faceNormal);

		// calculate a point on the triangle for the plane equation
		DirectX::XMVECTOR point = vertex0;

		// get plane equation (Ax + By + Cz + D = 0)
		float tri1A = DirectX::XMVectorGetX(faceNormal);
		float tri1B = DirectX::XMVectorGetY(faceNormal);
		float tri1C = DirectX::XMVectorGetZ(faceNormal);
		float tri1D = (-tri1A * DirectX::XMVectorGetX(point) - tri1B * DirectX::XMVectorGetY(point) - tri1C * DirectX::XMVectorGetZ(point));

		// get the length of the ray
		float ep1, ep2, t = 0.0f;
		float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
		DirectX::XMVECTOR pointInPlane = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		ep1 = (DirectX::XMVectorGetX(rayOrigin) * tri1A) + (DirectX::XMVectorGetY(rayOrigin) * tri1B) + (DirectX::XMVectorGetZ(rayOrigin) * tri1C);
		ep2 = (DirectX::XMVectorGetX(rayDirection) * tri1A) + (DirectX::XMVectorGetY(rayDirection) * tri1B) + (DirectX::XMVectorGetZ(rayDirection) * tri1C);

		//Make sure there are no divide-by-zeros
		if (ep2 != 0.0f)
		{
			t = -(ep1 + tri1D) / (ep2);
		}

		if (t > 0.0f)    //Make sure you don't pick objects behind the camera
		{
			//Get the point on the plane
			planeIntersectX = DirectX::XMVectorGetX(rayOrigin) + DirectX::XMVectorGetX(rayDirection) * t;
			planeIntersectY = DirectX::XMVectorGetY(rayOrigin) + DirectX::XMVectorGetY(rayDirection) * t;
			planeIntersectZ = DirectX::XMVectorGetZ(rayOrigin) + DirectX::XMVectorGetZ(rayDirection) * t;

			pointInPlane = DirectX::XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectZ, 0.0f);

			//Call function to check if point is in the triangle
			if (PointInTriangle(vertex0, vertex1, vertex2, pointInPlane))
			{
				return true;
			}
		}
	}

	return false;
}
bool PointInTriangle(DirectX::XMVECTOR & triV1, DirectX::XMVECTOR & triV2, DirectX::XMVECTOR & triV3, DirectX::XMVECTOR & point)
{
	DirectX::XMVECTOR cp1 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(triV3, triV2), DirectX::XMVectorSubtract(point, triV2));
	DirectX::XMVECTOR cp2 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(triV3, triV2), DirectX::XMVectorSubtract(triV1, triV2));

	if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(cp1, cp2)) >= 0)
	{
		cp1 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(triV3, triV1), DirectX::XMVectorSubtract(point, triV1));
		cp2 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(triV3, triV1), DirectX::XMVectorSubtract(triV2, triV1));

		if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(cp1, cp2)) >= 0)
		{
			cp1 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(triV2, triV1), DirectX::XMVectorSubtract(point, triV1));
			cp2 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(triV2, triV1), DirectX::XMVectorSubtract(triV3, triV1));

			if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(cp1, cp2)) >= 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return false;
}

DirectX::XMINT2 getfloorTile(POINT & mousePos, Window & window, Renderer & renderer, std::vector<CubeObject> & floor)
{
	GetCursorPos(&mousePos);
	ScreenToClient(window.getHandle(), &mousePos);

	DirectX::XMVECTOR rayOrigin = { 0 };
	DirectX::XMVECTOR rayDirection = { 0 };

	pickRayVector(renderer, mousePos.x, mousePos.y, rayOrigin, rayDirection);

	for (int i = 0; i < floor.size(); i++)
	{
		// if clicked on
		if (pick(rayOrigin, rayDirection, floor[i].getVertexPositions(), floor[i].getIndices(), floor[i].getWorldMatrix()))
		{
			return { (int)floor[i].position.x, (int)floor[i].position.z };
		}
	}

	return { -1, -1 };
}

DirectX::XMFLOAT3 findDistance(DirectX::XMFLOAT3 currentPos, DirectX::XMINT2 nextGridPos)
{
	// convert current position to int2
	DirectX::XMINT2 current = { (int)currentPos.x, (int)currentPos.z };
	DirectX::XMINT2 distance = { 0, 0 };

	// get distance between points
	if (current.x < nextGridPos.x)
	{
		distance.x = nextGridPos.x - current.x;
	}
	else
	{
		distance.x = current.x - nextGridPos.x;
	}

	if (current.y < nextGridPos.y)
	{
		distance.y = nextGridPos.y - current.y;
	}
	else
	{
		distance.y = current.y - nextGridPos.y;
	}

	// convert int2 back to float3
	return { (float)distance.x, 0.0f, (float)distance.y };
}