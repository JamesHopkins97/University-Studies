
//
// DXVertexBasic.cpp
//

#include <stdafx.h>
#include <DXVertexBasic.h>

// Vertex input descriptor based on DXVertexBasic
static const D3D11_INPUT_ELEMENT_DESC basicVertexDesc[] = {

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

// Create an input layout object mapping the vertex structure to the vertex shader input defined in the shader bytecode *shaderBlob
HRESULT DXVertexBasic::createInputLayout(ID3D11Device *device, char *shaderBytecode, uint32_t shaderSizeBytes, ID3D11InputLayout **layout) {	
	return device->CreateInputLayout(basicVertexDesc, ARRAYSIZE(basicVertexDesc), shaderBytecode, shaderSizeBytes, layout);
}
