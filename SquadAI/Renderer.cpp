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

	cbPerObjectBuffer->Release();
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

void Renderer::createConstantBuffer()
{
	// define constant buffer
	D3D11_BUFFER_DESC constantBufferDesc = { 0 };
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.ByteWidth = sizeof(cbPerObject);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// create constant buffer
	auto CCBresult = device->CreateBuffer(&constantBufferDesc, nullptr, &cbPerObjectBuffer);

	// check for errors
	if (CCBresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating constant buffer", "Error", MB_OK);
		exit(0);
	}

	//Camera information
	camPosition = DirectX::XMVectorSet(0.0f, 5.0f, -8.0f, 0.0f);
	camTarget = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// set view matrix
    camView = DirectX::XMMatrixLookAtLH(camPosition, camTarget, camUp);

	// set projection matrix
	camProjection = DirectX::XMMatrixPerspectiveFovLH(0.4f * DirectX::XM_PI, (float)backBufferDesc.Width / (float)backBufferDesc.Height, 1.0f, 1000.0f);
}

//set the clear background
void Renderer::beginFrame()
{
	// set background colour
	deviceContext->ClearRenderTargetView(renderTargetView, backgroundColour);

	// refresh depth stencil view
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::update()
{
	rot += 0.0005f;

	if (rot > 2 * DirectX::XM_PI)
	{
		rot = 0.0f;
	}

	// reset cube1World
	cube1World = DirectX::XMMatrixIdentity();

	// define cube1World
	DirectX::XMVECTOR rotaxis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	rotation = DirectX::XMMatrixRotationAxis(rotaxis, rot);
	translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, 4.0f);

	// set cube1World + transforms
	cube1World = translation * rotation;

	// reset cube2World
	cube2World = DirectX::XMMatrixIdentity();

	// define cube2World
	rotation = DirectX::XMMatrixRotationAxis(rotaxis, -rot);
	scale = DirectX::XMMatrixScaling(1.3f, 1.3f, 1.3f);

	// set cube1World + transforms
	cube2World = rotation * scale;
}

void Renderer::draw(UINT indexCount)
{
	//Set the WVP matrix and send it to the constant buffer in effect file
	WVP = cube1World * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	deviceContext->UpdateSubresource(cbPerObjectBuffer, 0, nullptr, &cbPerObj, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

	//Draw the first cube
	deviceContext->DrawIndexed(indexCount, 0, 0);

	WVP = cube2World * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	deviceContext->UpdateSubresource(cbPerObjectBuffer, 0, nullptr, &cbPerObj, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

	//Draw the second cube
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

// swap frame buffers
void Renderer::endFrame()
{
	swapChain->Present(0, 0);
}

// getters
ID3D11Device * Renderer::getDevice()
{
	return device;
}
ID3D11DeviceContext * Renderer::getDeviceContext()
{
	return deviceContext;
}
ID3D11DepthStencilView * Renderer::getDepthStencilView()
{
	return depthStencilView;
}
