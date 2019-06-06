#pragma once
#include <d3d11_2.h>
#include <DirectXMath.h>

class Texture;
class Material;
class Effect;

class Mesh
{
protected:
	Material *material = nullptr;
	Effect *effect = nullptr;
	UINT numVert=0;
	UINT numInd = 0;
	ID3D11Buffer				*vertexBuffer = nullptr;
	ID3D11Buffer				*indexBuffer = nullptr;
	//ID3D11InputLayout			*inputLayout = nullptr;
	// Augment grid with texture view
	ID3D11ShaderResourceView		*textureResourceView = nullptr;
	ID3D11SamplerState				*linearSampler = nullptr;

public:
	Mesh(ID3D11Device *device, Effect *_effect, ID3D11ShaderResourceView *tex_view, Material *_material);
	void render(ID3D11DeviceContext *context);
	~Mesh();
};

