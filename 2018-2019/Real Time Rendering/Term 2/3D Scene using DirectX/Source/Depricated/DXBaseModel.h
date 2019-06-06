
#pragma once

#include <d3d11_2.h>
#include <GUObject.h>


// Abstract base class to model mesh objects for rendering in DirectX
class DXBaseModel : public GUObject {

protected:

	ID3D11Buffer				*vertexBuffer = nullptr;
	ID3D11Buffer				*indexBuffer = nullptr;
	ID3D11InputLayout			*inputLayout = nullptr;

public:

	~DXBaseModel();

	virtual void render(ID3D11DeviceContext *context) = 0;
};
