#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "Window.h"

struct cbPerObject
{
	DirectX::XMMATRIX WVP;
};

class  Renderer
{
public:
	Renderer(Window& window, float clearColour[4]);
	~Renderer();

	void setClearColour(float clearColour[4]);

	void createDevice(Window& window);
	void createRenderTarget();
	void createDepthStencilBuffer();

	void setRenderTargets();
	void setViewport();

	void createConstantBuffer();

	void beginFrame();		// starts the frame clear
	void update();
	void draw(UINT indexCount);
	void endFrame();		// swap frame buffers

	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
	ID3D11DepthStencilView* getDepthStencilView();

private:

	float backgroundColour[4];

	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;

	ID3D11RenderTargetView* renderTargetView = nullptr;

	ID3D11Texture2D* backBuffer = nullptr;
	D3D11_TEXTURE2D_DESC backBufferDesc;

	ID3D11Texture2D* depthStencilBuffer = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;

	D3D11_VIEWPORT viewport;

	ID3D11Buffer* cbPerObjectBuffer;

	cbPerObject cbPerObj;

	DirectX::XMMATRIX WVP;

	DirectX::XMMATRIX camView;
	DirectX::XMMATRIX camProjection;

	DirectX::XMVECTOR camPosition;
	DirectX::XMVECTOR camTarget;
	DirectX::XMVECTOR camUp;

	DirectX::XMMATRIX cube1World;
	DirectX::XMMATRIX cube2World;

	DirectX::XMMATRIX rotation;
	DirectX::XMMATRIX scale;
	DirectX::XMMATRIX translation;
	float rot = 0.01f;
};
