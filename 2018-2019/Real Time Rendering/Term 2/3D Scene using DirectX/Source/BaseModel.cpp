
#include "stdafx.h"
#include <BaseModel.h>

BaseModel::BaseModel(ID3D11Device *device, Effect *_effect, Material *_materials[], int _numMaterials, ID3D11ShaderResourceView **_textures, int _numTextures) {



	if (_textures != nullptr && _numTextures == 0)_numTextures = 1;
	if (_materials != nullptr && _numMaterials == 0)_numMaterials = 1;
	
	if (_numTextures != 0) setTextures(_textures, _numTextures);
	if (_numMaterials != 0) setMaterials(_materials, _numMaterials);
	effect = _effect;
	inputLayout = effect->getVSInputLayout();
	createDefaultLinearSampler(device);
	initCBuffer(device);
	//init(device);
}

BaseModel::BaseModel(ID3D11Device *device, ID3D11InputLayout *_inputLayout, Material *_materials[], int _numMaterials, ID3D11ShaderResourceView **_textures, int _numTextures) {
	
	if (_textures != nullptr && _numTextures == 0)_numTextures = 1;
	if (_materials != nullptr && _numMaterials == 0)_numMaterials = 1;

	if (_numTextures != 0) setTextures(_textures, _numTextures);
	if (_numMaterials != 0) setMaterials(_materials, _numMaterials);
	inputLayout = _inputLayout;
	createDefaultLinearSampler(device);
	initCBuffer(device);
	
}
void BaseModel::initCBuffer(ID3D11Device *device){	
	
	// Allocate 16 byte aligned block of memory for "main memory" copy of cBufferBasic
	cBufferModelCPU = (CBufferModel*)_aligned_malloc(sizeof(CBufferModel), 16);

	// Fill out cBufferModelCPU

	cBufferModelCPU->worldMatrix = XMMatrixIdentity();
	cBufferModelCPU->worldITMatrix = XMMatrixIdentity();

	// Create GPU resource memory copy of cBufferBasic
	// fill out description (Note if we want to update the CBuffer we need  D3D11_CPU_ACCESS_WRITE)
	D3D11_BUFFER_DESC cbufferDesc;
	D3D11_SUBRESOURCE_DATA cbufferInitData;
	ZeroMemory(&cbufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&cbufferInitData, sizeof(D3D11_SUBRESOURCE_DATA));

	cbufferDesc.ByteWidth = sizeof(CBufferModel);
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferInitData.pSysMem = cBufferModelCPU;// Initialise GPU CBuffer with data from CPU CBuffer

	HRESULT hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData,
		&cBufferModelGPU);

}
void BaseModel::setWorldMatrix(XMMATRIX _worldMatrix){
	cBufferModelCPU->worldMatrix = _worldMatrix;
	XMVECTOR det=XMMatrixDeterminant(_worldMatrix);
	cBufferModelCPU->worldITMatrix = XMMatrixInverse(&det, XMMatrixTranspose(_worldMatrix));
}

void BaseModel::update(ID3D11DeviceContext *context) {
	mapCbuffer(context, cBufferModelCPU, cBufferModelGPU, sizeof(CBufferModel));
	context->PSSetConstantBuffers(0, 1, &cBufferModelGPU);
	context->VSSetConstantBuffers(0, 1, &cBufferModelGPU);
}

void BaseModel::createDefaultLinearSampler(ID3D11Device *device){
	
	// If textures are used a sampler is required for the pixel shader to sample the texture
	D3D11_SAMPLER_DESC linearDesc;

	ZeroMemory(&linearDesc, sizeof(D3D11_SAMPLER_DESC));
	linearDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	linearDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	linearDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	linearDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	linearDesc.MinLOD = 0.0f;
	linearDesc.MaxLOD = 0.0f;
	linearDesc.MipLODBias = 0.0f;
	linearDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	device->CreateSamplerState(&linearDesc, &sampler);
}

void BaseModel::setTextures(ID3D11ShaderResourceView *_textures[], int _numTextures){
	numTextures = _numTextures;
	for (int i = 0; i < numTextures; i++)
		textures[i] = _textures[i];
};


void BaseModel::setMaterials(Material *_materials[], int _numMaterials) {
	numMaterials = _numMaterials;
	materials[0] = _materials[0];
	for (int i = 0; i < numMaterials; i++)
		materials[i] = _materials[i];
};


BaseModel::~BaseModel() {

	if (vertexBuffer)
		vertexBuffer->Release();

	if (indexBuffer)
		indexBuffer->Release();

	if (inputLayout)
		inputLayout->Release();
}
