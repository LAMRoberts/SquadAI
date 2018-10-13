#pragma once
#include <DirectXMath.h>
#include <vector>
#include <fstream>

#include "Renderer.h"

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z, float r, float g, float b, float a) : position (x,y,z), colour (r,g,b,a) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 colour;
};

class Cube
{
public:
	Cube(Renderer& renderer);
	~Cube();

	UINT getVertexCount();
	UINT getIndexCount();

	void draw(Renderer& renderer);

private:

	void createVertexBuffer(Renderer & renderer);
	void createIndexBuffer(Renderer & renderer);

	void createVertexShader(Renderer & renderer);
	void createPixelShader(Renderer & renderer);

	void createInputLayout(Renderer & renderer);

	void createRenderStates(Renderer & renderer);
	
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	UINT vertexCount = 0;
	UINT indexCount = 0;

	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;

	ID3D11InputLayout* inputLayout = nullptr;

	ID3D11RasterizerState* rasterizerState = nullptr;

	std::vector<char> vsData;
	std::vector<char> psData;
};

