#include "Cube.h"

Cube::Cube(Renderer& renderer)
{
	createVertexBuffer(renderer);
	createIndexBuffer(renderer);

	createVertexShader(renderer);
	createPixelShader(renderer);

	createInputLayout(renderer);

	createRenderStates(renderer);

	renderer.getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

Cube::~Cube()
{
	m_vertexBuffer->Release();
	m_IndexBuffer->Release();

	m_vertexShader->Release();
	m_pixelShader->Release();

	m_inputLayout->Release();
}

void Cube::draw(Renderer & renderer)
{
	//bind triangle shaders
	renderer.getDeviceContext()->IASetInputLayout(m_inputLayout);

	// draw triangle
	renderer.getDeviceContext()->DrawIndexed(6, 0, 0);
}

Vertex Cube::createVertex(float x, float y, float z, float r, float g, float b)
{
	Vertex v =
	{
		DirectX::XMFLOAT3(x, y, z),
		DirectX::XMFLOAT3(r, g, b)
	};

	return v;
}

void Cube::createVertexBuffer(Renderer& renderer)
{
	// define vertices
	Vertex vertices[] =
	{
		createVertex(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f),
		createVertex(-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f),
		createVertex(0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f),

		createVertex(0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 1.0f),
		createVertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 1.0f),
		createVertex(0.5f, 0.5f, 0.0f, 0.0f, 0.5f, 1.0f),
	};

	// create vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// define vertex data
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = vertices;

	// create the vertex buffer with the device.
	auto CVBresult = renderer.getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// check for errors
	if (CVBresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating vertex buffer", "Error", MB_OK);
		exit(0);
	}

	// bind vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	renderer.getDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
}

void Cube::createIndexBuffer(Renderer& renderer)
{
	// define indices
	unsigned int indices[] = 
	{ 
		0, 1, 2,
		3, 4, 5,
	};

	// Fill in a buffer description.
	D3D11_BUFFER_DESC indexBufferDesc = { 0 };
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * ARRAYSIZE(indices);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	// define index data
	D3D11_SUBRESOURCE_DATA indexData = { 0 };
	indexData.pSysMem = indices;

	// create the index buffer with the device.
	auto CIBresult = renderer.getDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);

	// check for errors
	if (CIBresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating index buffer", "Error", MB_OK);
		exit(0);
	}

	// set the index buffer
	renderer.getDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Cube::createVertexShader(Renderer & renderer)
{
	// load file
	std::ifstream vsFile("VertexShader.cso", std::ios::binary);

	// load data
	vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };

	// create shaders
	auto CVSresult = renderer.getDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &m_vertexShader);

	// check for errors
	if (CVSresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating vertex shader", "Error", MB_OK);
		exit(0);
	}

	// bind vertex shader
	renderer.getDeviceContext()->VSSetShader(m_vertexShader, nullptr, 0);
}

void Cube::createPixelShader(Renderer & renderer)
{
	// load file
	std::ifstream psFile("PixelShader.cso", std::ios::binary);

	// load data
	psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

	// create shaders
	auto CPSresult = renderer.getDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &m_pixelShader);

	// check for errors
	if (CPSresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating pixel shader", "Error", MB_OK);
		exit(0);
	}

	// bind pixel shader
	renderer.getDeviceContext()->PSSetShader(m_pixelShader, nullptr, 0);
}

void Cube::createInputLayout(Renderer & renderer)
{
	// define input layouts
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// create layout
	auto CILrestult = renderer.getDevice()->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &m_inputLayout);

	// check for errors
	if (CILrestult != S_OK)
	{
		MessageBox(nullptr, "Problem creating input layout", "Error", MB_OK);
		exit(0);
	}
}

void Cube::createRenderStates(Renderer& renderer) 
{
	// Rasterizer state
	auto rasterizerDesc = CD3D11_RASTERIZER_DESC(
		D3D11_FILL_SOLID,
		D3D11_CULL_NONE,
		false, 0, 0, 0, 0, false, false, false);

	renderer.getDevice()->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
}