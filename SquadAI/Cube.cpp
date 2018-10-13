#include "Cube.h"

Cube::Cube(Renderer& renderer)
{
	createVertexBuffer(renderer);
	createIndexBuffer(renderer);

	createVertexShader(renderer);
	createPixelShader(renderer);

	createInputLayout(renderer);

	createRenderStates(renderer);

	//bind triangle shaders
	renderer.getDeviceContext()->IASetInputLayout(inputLayout);


	// set topology
	renderer.getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

Cube::~Cube()
{
	vertexBuffer->Release();
	indexBuffer->Release();

	vertexShader->Release();
	pixelShader->Release();

	inputLayout->Release();

	rasterizerState->Release();
}

UINT Cube::getVertexCount()
{
	return vertexCount;
}

UINT Cube::getIndexCount()
{
	return indexCount;
}

void Cube::createVertexBuffer(Renderer& renderer)
{
	// define vertices
	Vertex vertices[] =
	{
		Vertex(0.5f, 0.5f, 0.5f, 0.75f, 0.0f, 0.0f, 1.0f),
		Vertex(-0.5f, -0.5f, 0.5f, 0.75f, 0.0f, 0.0f, 1.0f),
		Vertex(-0.5f, 0.5f, 0.5f, 0.75f, 0.0f, 0.0f, 1.0f),

		Vertex(0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.75f, 1.0f),
		Vertex(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.75f, 1.0f),
		Vertex(0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.75f, 1.0f),

		Vertex(0.5f, 0.5f, 0.1f, 0.0f, 0.75f, 0.0f, 1.0f),
		Vertex(0.5f, -0.5f, 0.1f, 0.0f, 0.75f, 0.0f, 1.0f),
		Vertex(-0.5f, 0.5f, 0.1f, 0.0f, 0.75f, 0.0f, 1.0f),
	};

	vertexCount = ARRAYSIZE(vertices);

	// create vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// define vertex data
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = vertices;

	// create the vertex buffer with the device.
	auto CVBresult = renderer.getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// check for errors
	if (CVBresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating vertex buffer", "Error", MB_OK);
		exit(0);
	}

	// bind vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	renderer.getDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
}

void Cube::createIndexBuffer(Renderer& renderer)
{
	// define indices
	unsigned int indices[] = 
	{ 
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
	};

	indexCount = ARRAYSIZE(indices);

	// Fill in a buffer description.
	D3D11_BUFFER_DESC indexBufferDesc = { 0 };
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	// define index data
	D3D11_SUBRESOURCE_DATA indexData = { 0 };
	indexData.pSysMem = indices;

	// create the index buffer with the device.
	auto CIBresult = renderer.getDevice()->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// check for errors
	if (CIBresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating index buffer", "Error", MB_OK);
		exit(0);
	}

	// set the index buffer
	renderer.getDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Cube::createVertexShader(Renderer & renderer)
{
	// load file
	std::ifstream vsFile("VertexShader.cso", std::ios::binary);

	// load data
	vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };

	// create shaders
	auto CVSresult = renderer.getDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);

	// check for errors
	if (CVSresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating vertex shader", "Error", MB_OK);
		exit(0);
	}

	// bind vertex shader
	renderer.getDeviceContext()->VSSetShader(vertexShader, nullptr, 0);
}

void Cube::createPixelShader(Renderer & renderer)
{
	// load file
	std::ifstream psFile("PixelShader.cso", std::ios::binary);

	// load data
	psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

	// create shaders
	auto CPSresult = renderer.getDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);

	// check for errors
	if (CPSresult != S_OK)
	{
		MessageBox(nullptr, "Problem creating pixel shader", "Error", MB_OK);
		exit(0);
	}

	// bind pixel shader
	renderer.getDeviceContext()->PSSetShader(pixelShader, nullptr, 0);
}

void Cube::createInputLayout(Renderer & renderer)
{
	// define input layouts
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// create layout
	auto CILrestult = renderer.getDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), vsData.data(), vsData.size(), &inputLayout);

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

	renderer.getDevice()->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
}

void Cube::draw(Renderer & renderer)
{
	// refresh depth stencil view
	renderer.getDeviceContext()->ClearDepthStencilView(renderer.getDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// draw triangle
	renderer.getDeviceContext()->DrawIndexed(indexCount, 0, 0);
}