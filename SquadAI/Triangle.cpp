#include <fstream>
#include <vector>
#include <time.h>

#include "Triangle.h"

struct Vertex
{
	float x;
	float y;

	float r;
	float g;
	float b;
};

Triangle::Triangle(Renderer& renderer)
{
	srand(time(NULL));

	createMesh(renderer);
	createShaders(renderer);

	renderer.getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

Triangle::~Triangle()
{
	m_vertexBuffer->Release();
	m_vertexShader->Release();
	m_pixelShader->Release();
	m_inputLayout->Release();
}

void Triangle::draw(Renderer & renderer)
{
	//bind triangle shaders
	renderer.getDeviceContext()->IASetInputLayout(m_inputLayout);
	renderer.getDeviceContext()->VSSetShader(m_vertexShader, nullptr, 0);
	renderer.getDeviceContext()->PSSetShader(m_pixelShader, nullptr, 0);

	// bind vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	renderer.getDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// draw triangle
	renderer.getDeviceContext()->Draw(3, 0);
}

float Triangle::randomNumber()
{
	float rn = (float)(rand() % 1000) / 1000;
	return rn;
}

void Triangle::createMesh(Renderer& renderer)
{
	//define vertices
	Vertex vertices[] = {	// x, y, r, g, b
		{ -1, -1, randomNumber(), randomNumber(), randomNumber() },
		{ 0, 1, randomNumber(), randomNumber(), randomNumber() },
		{ 1, -1, randomNumber(), randomNumber(), randomNumber() }
	};

	// create vertex buffer
	auto vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);

	//create vertex data for vram
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = vertices;

	auto CVBresult = renderer.getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// check for errors
	if (CVBresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating vertex buffer", "Error", MB_OK);
		exit(0);
	}
}

void Triangle::createShaders(Renderer & renderer)
{
	// load file
	std::ifstream vsFile("triangleVertexShader.cso", std::ios::binary);
	std::ifstream psFile("trianglePixelShader.cso", std::ios::binary);

	// load data
	std::vector<char> vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	std::vector<char> psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

	// create shaders
	auto CVSresult = renderer.getDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &m_vertexShader);
	auto CPSresult = renderer.getDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &m_pixelShader);

	// check for errors
	if (CVSresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating vertex shader", "Error", MB_OK);
		exit(0);
	}
	if (CPSresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating pixel shader", "Error", MB_OK);
		exit(0);
	}

	// create input layouts
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	auto CILrestult = renderer.getDevice()->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &m_inputLayout);

	// check for errors
	if (CILrestult != S_OK)
	{
		MessageBox(nullptr, "Problem creating input layout", "Error", MB_OK);
		exit(0);
	}
}