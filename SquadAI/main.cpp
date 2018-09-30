#include <Windows.h>
#include "Window.h"
#include "Renderer.h"
#include "Triangle.h"

// weird windows version of main
int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
	//create window
	Window window(800, 600);

	//create renderer and init D3D
	Renderer renderer(window);

	// create triangle
	Triangle triangle(renderer);

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

		triangle.draw(renderer);

			// people all rendered
		renderer.endFrame();

	}

	return 0; // exited without a problem
}