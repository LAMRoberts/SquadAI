#include <Windows.h>

#include "Window.h"
#include "Renderer.h"
#include "Cube.h"

enum  Direction
{
	LEFT		= 0,
	RIGHT		= 1,
	UP			= 2,
	DOWN		= 3,
	FORWARD		= 4,
	BACK		= 5,
};

// weird windows version of main
int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
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

	//create message
	MSG msg = { 0 };

	bool rotate = false;

	std::vector<bool> direction;
	direction.assign(6, false);

	//main loop
	while (msg.message != WM_QUIT)
	{
		if (msg.message == WM_KEYDOWN)
		{
			// change character code into wParam output
			int c = msg.wParam + 32;

			switch (c)
			{
			case 'a':
			{
				direction[LEFT] = true;

				break;
			}
			case 'd':
			{
				direction[RIGHT] = true;

				break;
			}
			case 'w':
			{			
				direction[FORWARD] = true;

				break;
			}
			case 's':
			{
				direction[BACK] = true;

				break;
			}
			case 'e':
			{
				direction[UP] = true;

				break;
			}
			case 'q':
			{
				direction[DOWN] = true;

				break;
			}
			case 'r':
			{
				rotate = true;
				
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
			int c = msg.wParam + 32;

			switch (c)
			{
			case 'a':
			{
				direction[LEFT] = false;

				break;
			}
			case 'd':
			{
				direction[RIGHT] = false;

				break;
			}
			case 'w':
			{
				direction[FORWARD] = false;

				break;
			}
			case 's':
			{
				direction[BACK] = false;

				break;
			}
			case 'e':
			{
				direction[UP] = false;

				break;
			}
			case 'q':
			{
				direction[DOWN] = false;

				break;
			}
			case 'r':
			{
				rotate = false;

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

		// rotate cube while r is pressed
		if (rotate)
		{
			renderer.rotate();
		}

		// move camera based on direction keys pressed
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

		renderer.update();
		renderer.draw(indices);

		/////////////////////// END OF MAIN LOOP ///////////////////////

		// clean up frame
		renderer.endFrame();
	}

	// exit without a problem
	return 0; 
}