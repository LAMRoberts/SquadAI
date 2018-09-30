#include "Renderer.h"

Renderer::Renderer(Window& window)
{
	createDevice(window);
	createRenderTarget();
}

void Renderer::createDevice(Window& window) 
{
	// define swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };						// initialize swap chain
	swapChainDesc.BufferCount = 1;									// number of back buffers
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// buffer format 32bit colour
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// what the back buffer is used for
	swapChainDesc.OutputWindow = window.getHandle();				// where to shove all this shit
	swapChainDesc.SampleDesc.Count = 1;								// multisampling (anti-aliasing)
	swapChainDesc.Windowed = true;									// windowed or fullscreen

	// create swapchain device and context
	auto CDSCresult = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
		nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&m_swapChain, &m_device, nullptr, &m_deviceContext);

	// check for errors
	if (CDSCresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating device and swap chain", "Error", MB_OK);
		exit(0);
	}
}


void Renderer::createRenderTarget()
{
	// create back buffer
	ID3D11Texture2D* backBuffer;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

	// create target view
	auto CRTVresult = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);

	// check for errors
	if (CRTVresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating render target view", "Error", MB_OK);
		exit(0);
	}

	backBuffer->GetDesc(&m_backBufferDesc);

	backBuffer->Release();
}

//set the clear background
void Renderer::beginFrame()
{
	//bind render target
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

	// set viewport
	auto viewport = CD3D11_VIEWPORT(0.0f, 0.0f, (float) m_backBufferDesc.Width, (float) m_backBufferDesc.Height);
	m_deviceContext->RSSetViewports(1, &viewport);

	// set background colour
	float clearColour[] = { 0.25f, 0.5f, 1.0f, 1.0f };
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColour);
}

// swap frame buffers
void Renderer::endFrame()
{
	m_swapChain->Present(0, 0);
}

// getter for renderer device
ID3D11Device * Renderer::getDevice()
{
	return m_device;
}

// getter for renderer device context
ID3D11DeviceContext * Renderer::getDeviceContext()
{
	return m_deviceContext;
}
