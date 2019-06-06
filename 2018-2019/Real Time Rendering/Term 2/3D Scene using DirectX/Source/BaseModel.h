
#pragma once

#include <d3d11_2.h>
#include <Effect.h>
#include <Material.h>
#include <Texture.h>

#define MAX_TEXTURES 8
#define MAX_MATERIALS 8


// Abstract base class to model mesh objects for rendering in DirectX
class BaseModel {

protected:

	ID3D11Buffer				*vertexBuffer = nullptr;
	ID3D11Buffer				*indexBuffer = nullptr;
	ID3D11InputLayout			*inputLayout = nullptr;
	Effect						*effect = nullptr;
	Material					*materials[MAX_MATERIALS];
	ID3D11ShaderResourceView	*textures[MAX_TEXTURES];
	ID3D11SamplerState			*sampler = nullptr;
	int							numTextures = 0;
	int							numMaterials = 0;
	CBufferModel* cBufferModelCPU = nullptr;
	ID3D11Buffer *cBufferModelGPU = nullptr;

public:

	BaseModel(ID3D11Device *device, Effect *_effect, Material *_materials[] = nullptr, int _numMaterials = 0, ID3D11ShaderResourceView *_textures[] = nullptr, int _numTextures = 0);
	BaseModel(ID3D11Device *device, ID3D11InputLayout *_inputLayout, Material *_materials[] = nullptr, int _numMaterials = 0, ID3D11ShaderResourceView *_textures[] = nullptr, int _numTextures = 0);
	
	~BaseModel();

	virtual void render(ID3D11DeviceContext *context) = 0;
	virtual HRESULT init(ID3D11Device *device) = 0;
	void update(ID3D11DeviceContext *context);

	void setTextures(ID3D11ShaderResourceView *_texures[], int _numTextures = 1);
	void setMaterials(Material *_materials[], int _numMaterials = 1); 
	ID3D11ShaderResourceView* getTexture(int textureIndex=0){ return textures[textureIndex];};
	Material * getMaterial(int materialIndex = 0) {return materials[materialIndex];};
	void setEffect(Effect *_effect){ effect = _effect;};// effect must have the same input layout as the model
	int getEffect(Effect *_effect){ _effect = effect;};
	void initCBuffer(ID3D11Device *device);
	void createDefaultLinearSampler(ID3D11Device *device);
	void setWorldMatrix(XMMATRIX _worldMatrix);
	XMMATRIX getWorldMatrix(){ return cBufferModelCPU->worldMatrix; };

};
