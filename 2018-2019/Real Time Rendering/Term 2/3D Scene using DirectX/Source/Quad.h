#pragma once




class Quad
{
protected:
	ID3D11Buffer					*vertexBuffer = nullptr;
	ID3D11Buffer					*indexBuffer = nullptr;
	ID3D11InputLayout				*inputLayout = nullptr;
	ID3D11SamplerState				*linearSampler = nullptr;
public:

	Quad(ID3D11Device *device,  ID3D11InputLayout	*_inputLayout);
	~Quad();

	void render(ID3D11DeviceContext *context);
};