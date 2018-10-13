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

	void draw(Renderer& renderer);

private:

	void createVertexBuffer(Renderer & renderer);
	void createIndexBuffer(Renderer & renderer);

	void createVertexShader(Renderer & renderer);
	void createPixelShader(Renderer & renderer);

	void createInputLayout(Renderer & renderer);

	void createRenderStates(Renderer & renderer);
	
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;

	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;

	ID3D11InputLayout* m_inputLayout = nullptr;

	ID3D11RasterizerState* m_rasterizerState = nullptr;

	std::vector<char> vsData;
	std::vector<char> psData;
};

