#include "stdafx.h"
#include "Mesh.h"
#include "Material.h"
#include "Effect.h"
#include "VertexStructures.h"

Mesh::Mesh(ID3D11Device *device, Effect *_effect, ID3D11ShaderResourceView *_texView, Material *_material)
{
	effect = _effect;
	material = _material;
	//inputLayout = effect->getVSInputLayout();
	textureResourceView = _texView;

	if (textureResourceView)
		textureResourceView->AddRef(); // We didnt create it here but dont want it deleted by the creator untill we have deconstructed
	
	D3D11_SAMPLER_DESC samplerDesc;

	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = 0.0f;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	device->CreateSamplerState(&samplerDesc, &linearSampler);
}


Mesh::~Mesh()
{
}


void Mesh::render(ID3D11DeviceContext *context) {

	effect->bindPipeline(context);

	// set shader reflection map shaders for shark
	context->VSSetShader(effect->getVertexShader(), 0, 0);
	context->PSSetShader(effect->getPixelShader(), 0, 0);

	// Validate DXModel before rendering (see notes in constructor)
	if (!context || !vertexBuffer || !indexBuffer || !effect)
		return;

	// Set vertex layout
	context->IASetInputLayout(effect->getVSInputLayout());

	// Set DXModel vertex and index buffers for IA
	ID3D11Buffer* vertexBuffers[] = { vertexBuffer };
	UINT vertexStrides[] = { sizeof(ExtendedVertexStruct) };
	UINT vertexOffsets[] = { 0 };

	context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology for IA
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Bind texture resource views and texture sampler objects to the PS stage of the pipeline
	if (textureResourceView && linearSampler) {
		context->PSSetShaderResources(0, 1, &textureResourceView);
		context->PSSetSamplers(0, 1, &linearSampler);
	}

	// Draw Mesh
	context->DrawIndexed(numInd, 0, 0);
}
