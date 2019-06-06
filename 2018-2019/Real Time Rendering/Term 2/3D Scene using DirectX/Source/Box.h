#pragma once
#include <Effect.h>
#include <Material.h>
#include <BaseModel.h>




class Box : public BaseModel {

public:
	Box(ID3D11Device *device, Effect *_effect, Material *_materials[] = nullptr, int _numMaterials = 0, ID3D11ShaderResourceView **textures = nullptr, int numTextures = 0) : BaseModel(device, _effect, _materials, _numMaterials, textures, numTextures){ init(device); }
	Box(ID3D11Device *device, ID3D11InputLayout *_inputLayout, Material *_materials[] = nullptr, int _numMaterials = 0, ID3D11ShaderResourceView **textures = nullptr, int numTextures = 0) : BaseModel(device, _inputLayout, _materials, _numMaterials, textures, numTextures){ init(device); }
	~Box();

	void render(ID3D11DeviceContext *context);
	HRESULT init(ID3D11Device *device);

};