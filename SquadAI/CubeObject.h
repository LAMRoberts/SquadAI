#pragma once

#include "Renderer.h"

class CubeObject
{
public:
	CubeObject(Renderer & renderer);
	~CubeObject();

	void preUpdate(Renderer & renderer);

	void translate(float x, float y, float z);
	void rotate(float speed, float x, float y, float z);
	void scale(float x, float y, float z);

	void postUpdate(Renderer & renderer);

	DirectX::XMMATRIX updateWorldMatrix(Renderer & renderer);

private:

	DirectX::XMFLOAT3 pos;
	DirectX::XMMATRIX world;
	float rot = 0.01f;
	DirectX::XMMATRIX oRotation;
	DirectX::XMMATRIX oScale;
	DirectX::XMMATRIX oTranslation;
};