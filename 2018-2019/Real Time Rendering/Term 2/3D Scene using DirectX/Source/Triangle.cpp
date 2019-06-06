
#include <stdafx.h>
#include <Triangle.h>
#include <VertexStructures.h>
#include <iostream>
#include <exception>

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;


HRESULT Triangle::init(ID3D11Device *device) {

	//static const
		BasicVertexStruct  vertices[] = {

		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMCOLOR(1.0f, 0.0f, 0.0f, 1.0f) }
	};

	try
	{

		// Add code here (Setup triangle vertex buffer)
		// Setup triangle vertex buffer
		if (!device || !inputLayout)
			throw exception("Invalid parameters for triangle model instantiation");

		// Setup vertex buffer


		// Setup vertex buffer
		D3D11_BUFFER_DESC vertexDesc;
		D3D11_SUBRESOURCE_DATA vertexData;

		ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
		ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));

		vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexDesc.ByteWidth = sizeof(BasicVertexStruct) * 3;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexData.pSysMem = vertices;

		HRESULT hr = device->CreateBuffer(&vertexDesc, &vertexData, &vertexBuffer);













		if (!SUCCEEDED(hr))
			throw exception("Vertex buffer cannot be created");
	}

	catch (exception& e)
	{
		cout << "Triangle object could not be instantiated due to:\n";
		cout << e.what() << endl;

		if (vertexBuffer)
			vertexBuffer->Release();

		if (inputLayout)
			inputLayout->Release();

		vertexBuffer = nullptr;
		inputLayout = nullptr;
	}
}




void Triangle::render(ID3D11DeviceContext *context) {

	// Validate object before rendering 
	if (!context || !vertexBuffer || !inputLayout)
		return;
	
	if (effect)
		// Sets shaders, states
		effect->bindPipeline(context);

	// Bind texture resource views and texture sampler objects to the PS stage of the pipeline
	if (numTextures>0 && sampler) {

		context->PSSetShaderResources(0, numTextures, textures);
		context->PSSetSamplers(0, 1, &sampler);
	}


	// Set vertex layout
	context->IASetInputLayout(inputLayout);

	// Set vertex and index buffers for IA
	ID3D11Buffer* vertexBuffers[] = { vertexBuffer };
	UINT vertexStrides[] = { sizeof(BasicVertexStruct) };
	UINT vertexOffsets[] = { 0 };

	context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);

	// Set primitive topology for IA
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw triangle object
	// Note: Draw vertices in the buffer one after the other.  Not the most efficient approach (see duplication in the above vertex data)
	// This is shown here for demonstration purposes
	context->Draw(3, 0);
}


Triangle::~Triangle() {

	if (vertexBuffer)
		vertexBuffer->Release();
	if (inputLayout)
		inputLayout->Release();
}
