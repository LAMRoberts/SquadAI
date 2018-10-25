#include <Windows.h>
#include <stdlib.h>
#include <chrono>

#include "Window.h"
#include "Renderer.h"
#include "Cube.h"
#include "Squad.h"

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
void pickRayVector(Renderer & renderer, float mouseX, float mouseY, DirectX::XMVECTOR & pickRayInWorldSpacePos, DirectX::XMVECTOR & pickRayInWorldSpaceDir);
bool pick(DirectX::XMVECTOR pickRayInWorldSpacePos, DirectX::XMVECTOR pickRayInWorldSpaceDir, std::vector<DirectX::XMFLOAT3>& vertPosArray, std::vector<DWORD>& indexPosArray, DirectX::XMMATRIX& worldSpace);
bool PointInTriangle(DirectX::XMVECTOR & triV1, DirectX::XMVECTOR & triV2, DirectX::XMVECTOR & triV3, DirectX::XMVECTOR & point);

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

	// create triangle
	Cube cube(renderer);

	// finish init
	renderer.setViewport();
	renderer.createConstantBuffer();
	UINT indices = cube.getIndexCount();

	// tests for matrices
	bool shifting = false;
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

	// create message
	MSG msg = { 0 };

	// cam Directions
	std::vector<bool> camDirs;
	camDirs.assign(6, false);

	// set squad separation and ID numbers
	for (int i = 0; i < squads.size(); i++)
	{
		// separation
		int unitsInRow = 0;
		switch (squads[i].getFormation())
		{
		case Formation::SQUARE:
		{
			unitsInRow = sqrt(squads[i].getSquadSize());

			for (int j = 0; j < squads[i].getSquadSize(); j++)
			{
				squads[i].cubeObjs[j].translate(squadXOffset, 0.0f, 0.0f);
			}

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

			for (int j = 0; j < squads[i].getSquadSize(); j++)
			{
				squads[i].cubeObjs[j].translate(squadXOffset, 0.0f, 0.0f);
			}

			break;
		}
		case Formation::COLUMNS:
		{
			unitsInRow = squads[i].getFormationNumber();

			for (int j = 0; j < squads[i].getSquadSize(); j++)
			{
				squads[i].cubeObjs[j].translate(squadXOffset, 0.0f, 0.0f);
			}

			break;
		}
		default:
		{
			break;
		}
		}
		squadXOffset += unitsInRow + (unitsInRow - 1) + squadSeparation;

		// IDs
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

		// check if shifting
		if (msg.message == WM_KEYDOWN && msg.wParam == 16)
		{
			if (msg.wParam == 16)
			{
				shifting = true;
				highlightingUnit = true;
			}
		}
		else if (msg.message == WM_KEYUP && msg.wParam == 16)
		{
			shifting = false;
			highlightingUnit = false;
		}

		// check input
		if (msg.message == WM_KEYDOWN)
		{
			float speed = deltaTime * 0.00001f;

			switch (msg.wParam)
			{
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
			GetCursorPos(&mousePos);
			ScreenToClient(window.getHandle(), &mousePos);

			DirectX::XMVECTOR prwsPos, prwsDir;
			pickRayVector(renderer, mousePos.x, mousePos.y, prwsPos, prwsDir);

			// for each squad
			for (int i = 0; i < squads.size(); i++)
			{
				// for each unit in each squad
				for (int j = 0; i < squads[i].cubeObjs.size(); j++)
				{
					// if clicked on

					// each cube needds an array of its vertex positions, index
					if (pick(prwsPos, prwsDir, squads[i].cubeObjs[j].getVertexPositions(), squads[i].cubeObjs[j].getIndices(), squads[i].cubeObjs[j].getWorldMatrix()))
					{
						// set selected to this units ID
					}
				}
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

		// move squads
		for (int i = 0; i < squads.size(); i++)
		{
			for (int j = 0; j < squads[i].getSquadSize(); j++)
			{
				squads[i].cubeObjs[j].preUpdate(renderer);

				// translation
				if ((isUnitSelected(DirectX::XMINT2{ i, j }) && highlightingUnit) || (i == selected.x && !highlightingUnit))
				{ // if specific unit is selected or entire unit is selected
					squads[i].cubeObjs[j].translate(traDirs[X], traDirs[Y], traDirs[Z]);
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

void pickRayVector(Renderer & renderer, float mouseX, float mouseY, DirectX::XMVECTOR & pickRayInWorldSpacePos, DirectX::XMVECTOR & pickRayInWorldSpaceDir)
{
	DirectX::XMVECTOR pickRayInViewSpaceDir = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR pickRayInViewSpacePos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	float rayX = 0.0f;
	float rayY = 0.0f;
	float rayZ = 0.0f;

	// 2D screen position to 3D view position
	DirectX::XMVECTOR xv = renderer.camProjection.r[0];
	DirectX::XMVECTOR yv = renderer.camProjection.r[1];

	rayX = (((2.0f * mouseX) / windowWidth) - 1) / DirectX::XMVectorGetX(xv);
	rayY = -(((2.0f * mouseY) / windowHeight) - 1) / DirectX::XMVectorGetY(yv);
	rayZ = 1.0f;

	pickRayInViewSpaceDir = DirectX::XMVectorSet(rayX, rayY, rayZ, 0.0f);

	DirectX::XMMATRIX pickRayToWorldSpaceMatrix;
	DirectX::XMVECTOR defaultVector;    

	pickRayToWorldSpaceMatrix = DirectX::XMMatrixInverse(&defaultVector, renderer.camView);

	pickRayInWorldSpacePos = XMVector3TransformCoord(pickRayInViewSpacePos, pickRayToWorldSpaceMatrix);
	pickRayInWorldSpaceDir = XMVector3TransformNormal(pickRayInViewSpaceDir, pickRayToWorldSpaceMatrix);
}

bool pick(DirectX::XMVECTOR pickRayInWorldSpacePos, 
		DirectX::XMVECTOR pickRayInWorldSpaceDir,
		std::vector<DirectX::XMFLOAT3> & vertPosArray,
		std::vector<DWORD> & indexPosArray,
		DirectX::XMMATRIX & worldSpace)
{
	for (int i = 0; i < indexPosArray.size() / 3; i++)
	{
		//Triangle's vertices V1, V2, V3
		DirectX::XMVECTOR tri1V1 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR tri1V2 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR tri1V3 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		//Temporary 3d floats for each vertex
		DirectX::XMFLOAT3 tV1, tV2, tV3;

		//Get triangle 
		tV1 = vertPosArray[indexPosArray[(i * 3) + 0]];
		tV2 = vertPosArray[indexPosArray[(i * 3) + 1]];
		tV3 = vertPosArray[indexPosArray[(i * 3) + 2]];

		tri1V1 = DirectX::XMVectorSet(tV1.x, tV1.y, tV1.z, 0.0f);
		tri1V2 = DirectX::XMVectorSet(tV2.x, tV2.y, tV2.z, 0.0f);
		tri1V3 = DirectX::XMVectorSet(tV3.x, tV3.y, tV3.z, 0.0f);

		//Transform the vertices to world space
		tri1V1 = DirectX::XMVector3TransformCoord(tri1V1, worldSpace);
		tri1V2 = DirectX::XMVector3TransformCoord(tri1V2, worldSpace);
		tri1V3 = DirectX::XMVector3TransformCoord(tri1V3, worldSpace);

		//Find the normal using U, V coordinates (two edges)
		DirectX::XMVECTOR U = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR V = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR faceNormal = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		U = DirectX::XMVectorSubtract(tri1V2, tri1V1);
		V = DirectX::XMVectorSubtract(tri1V3, tri1V1);

		//Compute face normal by crossing U, V
		faceNormal = DirectX::XMVector3Cross(U, V);

		faceNormal = DirectX::XMVector3Normalize(faceNormal);

		//Calculate a point on the triangle for the plane equation
		DirectX::XMVECTOR triPoint = tri1V1;

		//Get plane equation ("Ax + By + Cz + D = 0") Variables
		float tri1A = DirectX::XMVectorGetX(faceNormal);
		float tri1B = DirectX::XMVectorGetY(faceNormal);
		float tri1C = DirectX::XMVectorGetZ(faceNormal);
		float tri1D = (-tri1A * DirectX::XMVectorGetX(triPoint) - tri1B * DirectX::XMVectorGetY(triPoint) - tri1C * DirectX::XMVectorGetZ(triPoint));

		//Now we find where (on the ray) the ray intersects with the triangles plane
		float ep1, ep2, t = 0.0f;
		float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
		DirectX::XMVECTOR pointInPlane = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		ep1 = (DirectX::XMVectorGetX(pickRayInWorldSpacePos) * tri1A) + (DirectX::XMVectorGetY(pickRayInWorldSpacePos) * tri1B) + (DirectX::XMVectorGetZ(pickRayInWorldSpacePos) * tri1C);
		ep2 = (DirectX::XMVectorGetX(pickRayInWorldSpaceDir) * tri1A) + (DirectX::XMVectorGetY(pickRayInWorldSpaceDir) * tri1B) + (DirectX::XMVectorGetZ(pickRayInWorldSpaceDir) * tri1C);

		//Make sure there are no divide-by-zeros
		if (ep2 != 0.0f)
		{
			t = -(ep1 + tri1D) / (ep2);
		}

		if (t > 0.0f)    //Make sure you don't pick objects behind the camera
		{
			//Get the point on the plane
			planeIntersectX = DirectX::XMVectorGetX(pickRayInWorldSpacePos) + DirectX::XMVectorGetX(pickRayInWorldSpaceDir) * t;
			planeIntersectY = DirectX::XMVectorGetY(pickRayInWorldSpacePos) + DirectX::XMVectorGetY(pickRayInWorldSpaceDir) * t;
			planeIntersectZ = DirectX::XMVectorGetZ(pickRayInWorldSpacePos) + DirectX::XMVectorGetZ(pickRayInWorldSpaceDir) * t;

			pointInPlane = DirectX::XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectZ, 0.0f);

			//Call function to check if point is in the triangle
			if (PointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
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
			cp1 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(triV2, triV1), (point, triV1));
			cp2 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(triV2, triV1), (triV3, triV1));

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