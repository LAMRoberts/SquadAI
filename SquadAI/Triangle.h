#pragma once
#include <vector>

#include "Renderer.h"

struct Vertex;

class Triangle
{
public:
	Triangle(Renderer& renderer);
	~Triangle();

	void draw(Renderer& renderer);

private:
	float randomNumber();

	Vertex createVertex(float x, float y, float z, float r, float g, float b);

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
};

