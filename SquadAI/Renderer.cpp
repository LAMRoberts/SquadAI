#include "Renderer.h"

Renderer::Renderer(Window& window, float clearColour[4])
{
	setClearColour(clearColour);
	createDevice(window);
	createRenderTarget();
	createDepthStencilBuffer();
	setRenderTargets();
}

Renderer::~Renderer()
{
	swapChain->Release();
	device->Release();
	deviceContext->Release();

	renderTargetView->Release();

	backBuffer->Release();

	depthStencilBuffer->Release();
	depthStencilView->Release();
}

void Renderer::setClearColour(float clearColour[4])
{
	for (int i = 0; i < 4; i++)
	{
		backgroundColour[i] = clearColour[i];
	}
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
		&swapChain, &device, nullptr, &deviceContext);

	// check for errors
	if (CDSCresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating with D3D init", "Error", MB_OK);
		exit(0);
	}
}

void Renderer::createRenderTarget()
{
	// create back buffer
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

	// create target view
	auto CRTVresult = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);

	// check for errors
	if (CRTVresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating render target view", "Error", MB_OK);
		exit(0);
	}

	backBuffer->GetDesc(&backBufferDesc);
}

void Renderer::createDepthStencilBuffer()
{
	//define depth stencil
	D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
	depthStencilDesc.Width = (float)backBufferDesc.Width;
	depthStencilDesc.Height = (float)backBufferDesc.Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// create depth stencil view
	auto CDSBresult = device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);

	// check for errors
	if (CDSBresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating depth stencil buffer", "Error", MB_OK);
		exit(0);
	}

	auto CDSVresult = device->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView);

	// check for errors
	if (CDSVresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating depth stencil view", "Error", MB_OK);
		exit(0);
	}
}

void Renderer::setRenderTargets()
{
	//bind render target
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

void Renderer::setViewport()
{
	//define viewport
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (float)backBufferDesc.Width;
	viewport.Height = (float)backBufferDesc.Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// set viewport
	deviceContext->RSSetViewports(1, &viewport);
}

//set the clear background
void Renderer::beginFrame()
{
	// set background colour
	deviceContext->ClearRenderTargetView(renderTargetView, backgroundColour);
}

// swap frame buffers
void Renderer::endFrame()
{
	swapChain->Present(0, 0);
}

// getter for renderer device
ID3D11Device * Renderer::getDevice()
{
	return device;
}

// getter for renderer device context
ID3D11DeviceContext * Renderer::getDeviceContext()
{
	return deviceContext;
}

ID3D11DepthStencilView * Renderer::getDepthStencilView()
{
	return depthStencilView;
}
