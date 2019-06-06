#pragma once
#include<Utils.h>
#include <BaseModel.h>
#include<Effect.h>
#include<VertexStructures.h>



class Flare : public BaseModel {

protected:

	// Create the indices
	bool visible = true;

	//BasicVertexStruct	*vertices = nullptr;

	//ID3D11ShaderResourceView *flareTextureSRV;
	//int numLights;
	//Effect *effect = nullptr;
	//ID3D11Buffer					*vertexBuffer = nullptr;
	//ID3D11InputLayout				*inputLayout = nullptr;
	//ID3D11Buffer						*cBufferGPU = nullptr;
	//CBufferParticles					*cBufferCPU = nullptr;

	//ID3D11SamplerState				*linearSampler = nullptr;
public:
	Flare(XMFLOAT3 position, XMCOLOR colour, ID3D11Device *device, Effect *_effect, Material *_materials[] = nullptr, int _numMaterials = 0, ID3D11ShaderResourceView **textures = nullptr, int numTextures = 0) : BaseModel(device, _effect, _materials, _numMaterials, textures, numTextures){ init(device, position,colour); }
	//Flare(ID3D11Device *device, Effect *_effect, ID3D11ShaderResourceView *_flareTextureSRV,);
	~Flare();
	void render(ID3D11DeviceContext *context);
	HRESULT init(ID3D11Device *device, XMFLOAT3 position, XMCOLOR colour);
	HRESULT init(ID3D11Device *device){ return S_OK; };
//	void render(ID3D11DeviceContext *context, Camera *camera);
	//void  update(ID3D11DeviceContext *context);
	//void setTexture(ID3D11ShaderResourceView *_flareTextureSRV){ flareTextureSRV = _flareTextureSRV; flareParticles->setTexture(flareTextureSRV); };
	//void setFlarePos(XMFLOAT3 _flarePos){ flarePos = _flarePos; };
	//void setSmokeOffset(XMFLOAT3 _smokeOffset){ smokeOffset = _smokeOffset; };
	//void setTimeShift(FLOAT _timeOffset = 0){timeOffset = _timeOffset;};

	//void setFlareAndSmokeSpeed(FLOAT _flareSpeedFactor = 2,FLOAT _smokeSpeedFactor = 1){
	//	 flareSpeedFactor = _flareSpeedFactor;smokeSpeedFactor = _smokeSpeedFactor;};
	//void setFlareAndSmokeScale(FLOAT _flareScaleFactor = 1, FLOAT _smokeScaleFactor = 2){
	//	flareScaleFactor = _flareScaleFactor; smokeScaleFactor = _smokeScaleFactor;
	//};
};

