#include "window.h"

// event catcher
LRESULT CALLBACK WinProc(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY || msg == WM_CLOSE)
	{
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(handle, msg, wparam, lparam);
}

// constructor and initializer for window
Window::Window(int width, int height)
{
	// define window style class
	WNDCLASS wc = { 0 };								// initialize class
	wc.style = CS_OWNDC;								// device context
	wc.lpfnWndProc = WinProc;							// callback event catcher
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);		// cursor
	wc.lpszClassName = "SquadAI";						// class name
	RegisterClass(&wc);									// register class

	//create window
	m_handle = CreateWindow(
		"SquadAI",											// window style class
		"DirectX11 Squad AI",								// window name
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,					// optional styles
		100, 100, width, height,							// position and size
		nullptr, nullptr, nullptr, nullptr);				// fuck only knows
}

// getter for the window handle
HWND Window::getHandle()
{
	return m_handle;
}