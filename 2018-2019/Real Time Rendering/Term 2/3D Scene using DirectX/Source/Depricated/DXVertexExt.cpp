
//
// DXVertexExt.cpp
//

#include <stdafx.h>
#include <DXVertexExt.h>



// Vertex input descriptor based on DXVertexExt
static const D3D11_INPUT_ELEMENT_DESC extVertexDesc[] = {

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "DIFFUSE", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SPECULAR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


// Create an input layout object mapping the vertex structure to the vertex shader input defined in the shader bytecode *shaderBlob
HRESULT DXVertexExt::createInputLayout(ID3D11Device *device, char *shaderByteCode, uint32_t shaderSizeBytes, ID3D11InputLayout **layout) {

	return device->CreateInputLayout(extVertexDesc, ARRAYSIZE(extVertexDesc), shaderByteCode, shaderSizeBytes, layout);
}
