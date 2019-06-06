
//
// DXVertexExt.cpp
//

#include <stdafx.h>
#include <DXVertexParticle.h>



// Vertex input descriptor based on DXVertexParticle
static const D3D11_INPUT_ELEMENT_DESC particleVertexDesc[] = {
	// Add Code Here (setup particle vertex description)
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "LPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "DATA", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }

};


// Create an input layout object mapping the vertex structure to the vertex shader input defined in the shader bytecode *shaderBlob
HRESULT DXVertexParticle::createInputLayout(ID3D11Device *device, char *shaderBytecode, uint32_t shaderSizeBytes, ID3D11InputLayout **layout) {

	return device->CreateInputLayout(particleVertexDesc, ARRAYSIZE(particleVertexDesc), shaderBytecode, shaderSizeBytes, layout);
}
