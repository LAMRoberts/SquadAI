#include <Windows.h>
#include <stdlib.h>
#include <time.h>

#include "Window.h"
#include "Renderer.h"
#include "Cube.h"
#include "Squad.h"

enum Direction
{
	LEFT		= 0,
	RIGHT		= 1,
	UP			= 2,
	DOWN		= 3,
	FORWARD		= 4,
	BACK		= 5,
};
enum Axis
{
	X = 0,
	Y = 1,
	Z = 2
};

// ewwww global members
UINT selectedSquad = 0;
UINT selectedUnit = 0;

// forward declarations
void moveCamera(Renderer & renderer, std::vector<bool> direction);
bool isUnitSelected(UINT squad, UINT unit);

// weird windows version of main
int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
	srand(time(NULL));

	//create window
	Window window(800, 800);

	//create renderer and init D3D
	float clearColour[] = { 0.2f, 0.3f, 0.5f, 1.0f };
	Renderer renderer(window, clearColour);

	// create triangle
	Cube cube(renderer);

	// finish init
	renderer.setViewport();
	renderer.createConstantBuffer();
	UINT indices = cube.getIndexCount();

	bool shifting = false;

	bool rotate = false;

	std::vector<float> traDirs;
	traDirs.assign(3, 0.0f);
	float traSpeed = 0.0002;

	bool scale = false;

	// init squad prefab with units in formation
	Squad defaultSquad(renderer, 100, Formation::COLUMNS, 5);

	// init 3 squads of 9 units
	std::vector<Squad> squads;
	squads.assign(1, defaultSquad);

	// create message
	MSG msg = { 0 };

	// cam Directions
	std::vector<bool> camDirs;
	camDirs.assign(6, false);

	// set squad separation
	for (int i = 0; i < squads.size(); i++)
	{
		for (int j = 0; j < squads[i].getSquadSize(); j++)
		{
			squads[i].cubeObjs[j].translate(i * 10.0f, 0.0f, 0.0f);
		}
	}

	// main loop
	while (msg.message != WM_QUIT)
	{
		// check if shifting
		if (msg.message == WM_KEYDOWN && msg.wParam == 16)
		{
			if (msg.wParam == 16)
			{
				shifting = true;
			}
		}
		else if (msg.message == WM_KEYUP && msg.wParam == 16)
		{
			shifting = false;
		}

		// check key
		if (msg.message == WM_KEYDOWN)
		{
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
				traDirs[X] = -traSpeed;

				break;
			}
			case VK_RIGHT:
			{
				traDirs[X] = traSpeed;

				break;
			}
			case VK_UP:
			{
				traDirs[Z] = traSpeed;

				break;
			}
			case VK_DOWN:
			{
				traDirs[Z] = -traSpeed;

				break;
			}
			case 49: // 1
			{
				if (shifting)
				{
					selectedSquad = 0;
				}
				else
				{
					selectedUnit = 0;
				}

				break;
			}
			case 50: // 2
			{
				if (shifting)
				{
					selectedSquad = 1;
				}
				else
				{
					selectedUnit = 1;
				}

				break;
			}
			case 51: // 3
			{
				if (shifting)
				{
					selectedSquad = 2;
				}
				else
				{
					selectedUnit = 2;
				}

				break;
			}
			case 52: // 4
			{
				if (shifting)
				{
					selectedSquad = 3;
				}
				else
				{
					selectedUnit = 3;
				}

				break;
			}
			case 53: // 5
			{
				if (shifting)
				{
					selectedSquad = 4;
				}
				else
				{
					selectedUnit = 4;
				}

				break;
			}
			case 54: // 6
			{
				if (shifting)
				{
					selectedSquad = 5;
				}
				else
				{
					selectedUnit = 5;
				}

				break;
			}
			case 55: // 7
			{
				if (shifting)
				{
					selectedSquad = 6;
				}
				else
				{
					selectedUnit = 6;
				}

				break;
			}
			case 56: // 8
			{
				if (shifting)
				{
					selectedSquad = 7;
				}
				else
				{
					selectedUnit = 7;
				}

				break;
			}
			case 57: // 9
			{
				if (shifting)
				{
					selectedSquad = 8;
				}
				else
				{
					selectedUnit = 8;
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

		// deal with message
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		for (int i = 0; i < squads.size(); i++)
		{
			for (int j = 0; j < squads[i].cubeObjs.size(); j++)
			{
				if (isUnitSelected(i, j))
				{
					squads[i].cubeObjs[j].scale(1.0f, 2.0f, 1.0f);
				}
				else if (i == selectedSquad)
				{
					squads[i].cubeObjs[j].scale(1.0f, 1.5f, 1.0f);
				}
			}
		}

		//start frame
		renderer.beginFrame();

		/////////////////////// START OF MAIN LOOP ///////////////////////

		// move camera based on direction keys pressed
		moveCamera(renderer, camDirs);

		for (int i = 0; i < squads.size(); i++)
		{
			for (int j = 0; j < squads[i].getSquadSize(); j++)
			{
				squads[i].cubeObjs[j].preUpdate(renderer);

				if (isUnitSelected(i, j))
				{
					squads[i].cubeObjs[j].translate(traDirs[X], traDirs[Y], traDirs[Z]);
				}

				if (rotate && isUnitSelected(i, j))
				{
					squads[i].cubeObjs[j].rotate(0.0002f, 0.0f, 1.0f, 0.0f);
				}
				
				if (i != selectedSquad)
				{
					squads[i].cubeObjs[j].scale(1.0f, 1.0f, 1.0f);
				}

				squads[i].cubeObjs[j].postUpdate(renderer);

				renderer.draw(indices);
			}
		}		

		/////////////////////// END OF MAIN LOOP ///////////////////////

		// clean up frame
		renderer.endFrame();
	}

	// exit without a problem
	return 0; 
}

void moveCamera(Renderer & renderer, std::vector<bool> direction)
{
	if (direction[LEFT])
	{
		renderer.moveCamera(-0.005f, 0.0f, 0.0f);
	}
	if (direction[RIGHT])
	{
		renderer.moveCamera(0.005f, 0.0f, 0.0f);
	}
	if (direction[UP])
	{
		renderer.moveCamera(0.0f, 0.005f, 0.0f);
	}
	if (direction[DOWN])
	{
		renderer.moveCamera(0.0f, -0.005f, 0.0f);
	}
	if (direction[FORWARD])
	{
		renderer.moveCamera(0.0f, 0.0f, 0.005f);
	}
	if (direction[BACK])
	{
		renderer.moveCamera(0.0f, 0.0f, -0.005f);
	}
}

bool isUnitSelected(UINT squad, UINT unit)
{
	if (squad == selectedSquad && unit == selectedUnit)
	{
		return true;
	}
	else
	{
		return false;
	}
}