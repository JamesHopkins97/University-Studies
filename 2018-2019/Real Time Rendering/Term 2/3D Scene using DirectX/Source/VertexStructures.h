#pragma once
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

struct FlareVertexStruct {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 posL;
	DirectX::PackedVector::XMCOLOR		colour;
};

// Vertex input descriptor based on Flare VertexStruct
static const D3D11_INPUT_ELEMENT_DESC flareVertexDesc[] = {
{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
{ "LPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

struct BasicVertexStruct {
	DirectX::XMFLOAT3					pos;
	DirectX::PackedVector::XMCOLOR		colour;
};

// Add code here (Create an input element description)
// Vertex input descriptor based on BasicVertexStruct

static const D3D11_INPUT_ELEMENT_DESC basicVertexDesc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


struct ExtendedVertexStruct {
	DirectX::XMFLOAT3					pos;
	DirectX::XMFLOAT3					normal;
	DirectX::PackedVector::XMCOLOR		matDiffuse;
	DirectX::PackedVector::XMCOLOR		matSpecular;
	DirectX::XMFLOAT2					texCoord;
};

// Vertex input descriptor based on ExtendedVertexStruct
static const D3D11_INPUT_ELEMENT_DESC extVertexDesc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "DIFFUSE", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "SPECULAR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


struct ParticleVertexStruct {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 posL;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 data;//;[age,?,?]
};

// Vertex input descriptor based on ParticleVertexStruct
static const D3D11_INPUT_ELEMENT_DESC particleVertexDesc[] = {
	// Add Code Here (setup particle vertex description)
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "LPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "DATA", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
