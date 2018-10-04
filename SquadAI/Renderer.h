#pragma once
#include <d3d11.h>
#include "Window.h"

class  Renderer
{
public:
	Renderer(Window& window, float clearColour[4]);

	void setClearColour(float clearColour[4]);

	void beginFrame();		// starts the frame clear
	void endFrame();		// swap frame buffers

	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();

private:

	void createDevice(Window& window);
	void createRenderTarget();

	float backgroundColour[4];

	// device members
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;

	// render target texture
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	D3D11_TEXTURE2D_DESC m_backBufferDesc;
};
