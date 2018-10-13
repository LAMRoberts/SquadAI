#include <Windows.h>
#include "Window.h"
#include "Renderer.h"
#include "Cube.h"

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

	renderer.setViewport();

	//create message
	MSG msg = { 0 };

	//main loop
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		renderer.beginFrame();
			// render all the people

		cube.draw(renderer);

			// people all rendered
		renderer.endFrame();

	}

	return 0; // exited without a problem
}