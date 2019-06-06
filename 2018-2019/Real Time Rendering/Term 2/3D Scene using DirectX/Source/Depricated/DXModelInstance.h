
#pragma once

#include <d3d11_2.h>
#include <DirectXMath.h>
#include <GUObject.h>

class DXBaseModel;

class DXModelInstance : public GUObject {

	DirectX::XMFLOAT3			T = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// Position (x, y, z)
	DirectX::XMFLOAT3			E = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// Euler angles in radians
	DXBaseModel					*model = nullptr;							// Reference to mesh model

public:

	DXModelInstance(DXBaseModel *_model, const DirectX::XMFLOAT3& initT, const DirectX::XMFLOAT3& initE);
	~DXModelInstance();

	void translate(const DirectX::XMFLOAT3& dT);
	void rotate(const DirectX::XMFLOAT3& dE);
	void setupCBuffer(ID3D11DeviceContext *context, ID3D11Buffer *cbuffer);
	void setupCBuffer(ID3D11DeviceContext *context, ID3D11Buffer *cbuffer, DirectX::XMFLOAT3& dS);
	void render(ID3D11DeviceContext *context);
};
