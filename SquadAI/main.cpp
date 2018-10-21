#include <Windows.h>
#include <stdlib.h>
#include <time.h>

#include "Window.h"
#include "Renderer.h"
#include "Cube.h"
#include "CubeObject.h"

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

void moveCamera(Renderer & renderer, std::vector<bool> direction);

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

	CubeObject cubeObj(renderer);
	std::vector<CubeObject> cubes;
	cubes.assign(9, cubeObj);

	//create message
	MSG msg = { 0 };

	UINT selectedCube = 0;

	// cam Directions
	std::vector<bool> camDirs;
	camDirs.assign(6, false);

	// rotation
	bool rotate = false;

	// translation
	float traSpeed = 0.0002;
	std::vector<float> traDirs;
	traDirs.assign(3, 0.0f);

	// scale
	bool scale = false;

	// init cube pos's
	for (int i = 0; i < cubes.size(); i++)
	{
		cubes[i].translate(0.0f, 0.0f, 2 * i);
	}

	//main loop
	while (msg.message != WM_QUIT)
	{
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
				selectedCube = 0;

				break;
			}
			case 50: // 2
			{
				selectedCube = 1;

				break;
			}
			case 51: // 3
			{
				selectedCube = 2;

				break;
			}
			case 52: // 4
			{
				selectedCube = 3;

				break;
			}
			case 53: // 5
			{
				selectedCube = 4;

				break;
			}
			case 54: // 6
			{
				selectedCube = 5;

				break;
			}
			case 55: // 7
			{
				selectedCube = 6;

				break;
			}
			case 56: // 8
			{
				selectedCube = 7;

				break;
			}
			case 57: // 9
			{
				selectedCube = 8;

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

		//start frame
		renderer.beginFrame();

		/////////////////////// START OF MAIN LOOP ///////////////////////

		// move camera based on direction keys pressed
		moveCamera(renderer, camDirs);

		cubes[selectedCube].scale(1.0f, 2.0f, 1.0f);

		for (int i = 0; i < cubes.size(); i++)
		{
			cubes[i].preUpdate(renderer);

			cubes[selectedCube].translate(traDirs[X], traDirs[Y], traDirs[Z]);

			if (rotate)
			{
				cubes[selectedCube].rotate(0.0002f, 0.0f, 1.0f, 0.0f);
			}

			if (i != selectedCube)
			{
				cubes[i].scale(1.0f, 1.0f, 1.0f);
			}

			cubes[i].postUpdate(renderer);

			renderer.draw(indices);
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