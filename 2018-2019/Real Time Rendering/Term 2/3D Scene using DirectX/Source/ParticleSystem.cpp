
#include <stdafx.h>
#include <ParticleSystem.h>
#include <iostream>
#include <exception>
#include <Material.h>

using namespace std;
using namespace DirectX;





HRESULT ParticleSystem::init(ID3D11Device* device)
{

	ParticleVertexStruct* vertices = (ParticleVertexStruct*)malloc(sizeof(ParticleVertexStruct) * N_VERT);
	UINT* indices = (UINT*)malloc(sizeof(UINT) * N_P_IND);

	try
	{
		//INITIALISE Verticies

		for (int i = 0; i < (N_VERT); i += 4)
		{
			vertices[i + 0].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[i + 0].posL = XMFLOAT3(-1.0f, -1.0f, 0.0f);
			vertices[i + 0].velocity = XMFLOAT3(((FLOAT)rand() / RAND_MAX) - 0.5, (FLOAT)rand() / RAND_MAX, ((FLOAT)rand() / RAND_MAX) - 0.5);
			vertices[i + 0].data = XMFLOAT3((FLOAT)rand() / RAND_MAX, 0.0f, 0.0f);

			vertices[i + 1].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[i + 1].posL = XMFLOAT3(-1.0f, 1.0f, 0.0f);
			vertices[i + 1].velocity = vertices[i + 0].velocity;
			vertices[i + 1].data = vertices[i + 0].data;

			vertices[i + 2].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[i + 2].posL = XMFLOAT3(1.0f, 1.0f, 0.0f);
			vertices[i + 2].velocity = vertices[i + 0].velocity;
			vertices[i + 2].data = vertices[i + 0].data;

			vertices[i + 3].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[i + 3].posL = XMFLOAT3(1.0f, -1.0f, 0.0f);
			vertices[i + 3].velocity = vertices[i + 0].velocity;
			vertices[i + 3].data = vertices[i + 0].data;

		}


		// Setup particles vertex buffer


		if (!device || !effect)
			throw exception("Invalid parameters for particles instantiation");

		// Setup vertex buffer
		D3D11_BUFFER_DESC vertexDesc;
		D3D11_SUBRESOURCE_DATA vertexdata;

		ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
		ZeroMemory(&vertexdata, sizeof(D3D11_SUBRESOURCE_DATA));

		vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexDesc.ByteWidth = sizeof(ParticleVertexStruct) * N_VERT;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexdata.pSysMem = vertices;

		HRESULT hr = device->CreateBuffer(&vertexDesc, &vertexdata, &vertexBuffer);

		if (!SUCCEEDED(hr))
			throw exception("Vertex buffer cannot be created");

		// Create the index buffer



		//INITIALISE Indicies

		for (int i = 0; i < N_PART; i++)
		{

			indices[(i * 6) + 0] = (i * 4) + 0;
			indices[(i * 6) + 1] = (i * 4) + 1;
			indices[(i * 6) + 2] = (i * 4) + 2;

			indices[(i * 6) + 3] = (i * 4) + 2;
			indices[(i * 6) + 4] = (i * 4) + 3;
			indices[(i * 6) + 5] = (i * 4) + 0;
		}


		D3D11_BUFFER_DESC indexDesc;
		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexDesc.ByteWidth = sizeof(UINT) * N_P_IND;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.CPUAccessFlags = 0;
		indexDesc.MiscFlags = 0;
		indexDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA indexdata;
		indexdata.pSysMem = indices;

		hr = device->CreateBuffer(&indexDesc, &indexdata, &indexBuffer);

		if (!SUCCEEDED(hr))
			throw exception("index buffer cannot be created");

		if (!SUCCEEDED(hr))
			throw exception("Cannot create input layout interface");


	}
	catch (exception& e)
	{
		cout << "Particles object could not be instantiated due to:\n";
		cout << e.what() << endl;

		if (vertexBuffer)
			vertexBuffer->Release();

		if (inputLayout)
			inputLayout->Release();

		vertexBuffer = nullptr;
		inputLayout = nullptr;
		indexBuffer = nullptr;
	}
}


ParticleSystem::~ParticleSystem() {

	if (vertexBuffer)
		vertexBuffer->Release();
	if (indexBuffer)
		indexBuffer->Release();
	if (inputLayout)
		inputLayout->Release();


}


void ParticleSystem::render(ID3D11DeviceContext* context) {




	context->PSSetConstantBuffers(0, 1, &cBufferModelGPU);
	context->VSSetConstantBuffers(0, 1, &cBufferModelGPU);

	// Validate object before rendering 
	if (!context || !vertexBuffer || !inputLayout)
		return;

	if (effect)
		// Sets shaders, states
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
	UINT vertexStrides[] = { sizeof(ParticleVertexStruct) };
	UINT vertexOffsets[] = { 0 };

	context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology for IA
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	// Draw particles object using index buffer
	// indices for the particles.
	context->DrawIndexed(N_P_IND, 0, 0);
}

