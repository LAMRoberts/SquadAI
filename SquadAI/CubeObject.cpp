#include "CubeObject.h"

CubeObject::CubeObject(Renderer & renderer)
{
	preUpdate(renderer);

	pos = { 0.0f, 0.0f, 0.0f };

	translate(0.0f, 0.0f, 0.0f);
	rotate(0.0f, 0.0f, 0.0f, 1.0f);
	scale(1.0f, 1.0f, 1.0f);

	postUpdate(renderer);

	renderer.updateWVP(updateWorldMatrix(renderer));
}

CubeObject::~CubeObject()
{

}

void CubeObject::preUpdate(Renderer & renderer)
{
	world = DirectX::XMMatrixIdentity();
}

void CubeObject::translate(float x, float y, float z)
{
	pos.x += x;
	pos.y += y;
	pos.z += z;

	oTranslation = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void CubeObject::rotate(float speed, float x, float y, float z)
{
	rot += speed;

	if (rot > 2.0f * DirectX::XM_PI || rot < -(2.0f * DirectX::XM_PI))
	{
		rot = 0.0f;
	}

	DirectX::XMVECTOR rotaxis = DirectX::XMVectorSet(x, y, z, 1.0f);

	oRotation = DirectX::XMMatrixRotationAxis(rotaxis, rot);
}

void CubeObject::scale(float x, float y, float z)
{
	oScale = DirectX::XMMatrixScaling(x, y, z);
}

void CubeObject::postUpdate(Renderer & renderer)
{
	// set world + transforms
	world = oRotation * oTranslation * oScale;

	renderer.updateWVP(updateWorldMatrix(renderer));
}

DirectX::XMMATRIX CubeObject::updateWorldMatrix(Renderer & renderer)
{
	//Set the WVP matrix and send it to the constant buffer in effect file
	DirectX::XMMATRIX m = world * renderer.camView * renderer.camProjection;
	renderer.cbPerObj.WVP = XMMatrixTranspose(m);
	renderer.getDeviceContext()->UpdateSubresource(renderer.cbPerObjectBuffer, 0, nullptr, &renderer.cbPerObj, 0, 0);
	renderer.getDeviceContext()->VSSetConstantBuffers(0, 1, &renderer.cbPerObjectBuffer);

	return m;
}

void CubeObject::setID(DirectX::XMINT2 unitID)
{
	ID = unitID;
}
DirectX::XMINT2 CubeObject::getID()
{
	return ID;
}

DirectX::XMMATRIX CubeObject::getWorldMatrix()
{
	return world;
}
