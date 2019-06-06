#pragma once
#include "VertexStructures.h"
#include <BaseModel.h>

class DXBlob;


class ParticleSystem : public BaseModel {


int N_PART = 50;
int N_VERT = N_PART * 4;
int N_P_IND = N_PART * 6;

public:
	ParticleSystem( ID3D11Device *device, Effect *_effect, Material *_materials[] = nullptr, int _numMaterials = 0, ID3D11ShaderResourceView **textures = nullptr, int numTextures = 0) : BaseModel(device, _effect, _materials, _numMaterials, textures, numTextures){ init(device); }

	~ParticleSystem();
	
	HRESULT init(ID3D11Device *device);

	void render(ID3D11DeviceContext *context);
};