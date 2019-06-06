#include "stdafx.h"
#include "Flare.h"


HRESULT Flare::init(ID3D11Device *device, XMFLOAT3 position, XMCOLOR colour)
{




	FlareVertexStruct vertices[] = {

		{ position, XMFLOAT3(-1.0f, -1.0f, 0.0f), colour },
		{ position, XMFLOAT3(-1.0f, 1.0f, 0.0f), colour },
		{ position, XMFLOAT3(1.0f, -1.0f, 0.0f), colour },
		{ position, XMFLOAT3(1.0f, 1.0f, 0.0f), colour }

	};

		// Setup flare vertex buffer
		// Setup vertex buffer
		D3D11_BUFFER_DESC vertexDesc;
		D3D11_SUBRESOURCE_DATA vertexdata;

		ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
		ZeroMemory(&vertexdata, sizeof(D3D11_SUBRESOURCE_DATA));

		vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexDesc.ByteWidth = sizeof(FlareVertexStruct )*4;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexdata.pSysMem = vertices;

		HRESULT hr = device->CreateBuffer(&vertexDesc, &vertexdata, &vertexBuffer);
		return S_OK;
}



Flare::~Flare()
{
}




void Flare::render(ID3D11DeviceContext *context)
{
	// Validate object before rendering (see notes in constructor)
	if (!context || !vertexBuffer || !effect)
		return;

	effect->bindPipeline(context);

	// Bind texture resource views and texture sampler objects to the PS stage of the pipeline
	if (numTextures > 0 && sampler) {

		context->PSSetShaderResources(0, numTextures, textures);
		context->PSSetSamplers(0, 1, &sampler);
	}

	// Set vertex layout
	context->IASetInputLayout(inputLayout);

	// Set vertex and index buffers for IA
	ID3D11Buffer* vertexBuffers[] = { vertexBuffer };
	UINT vertexStrides[] = { sizeof(FlareVertexStruct) };
	UINT vertexOffsets[] = { 0 };

	context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);


	// Set primitive topology for IA
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	context->Draw(4, 0);
}
