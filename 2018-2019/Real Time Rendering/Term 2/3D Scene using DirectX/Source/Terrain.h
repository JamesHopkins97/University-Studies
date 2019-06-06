#pragma once
#include "BaseModel.h"
#include "CBufferStructures.h"
#include "VertexStructures.h"
#include "Camera.h"
class Effect;
class Material;
//#include <DirectXMath.h>

class Terrain : public BaseModel
{

	int width, height;
	UINT numInd = 0;
public:
	Terrain(ID3D11Device *device, ID3D11DeviceContext*context, int width, int height, ID3D11Texture2D*tex_height, ID3D11Texture2D*tex_normal,  Effect *_effect, Material *_materials[] = nullptr, int _numMaterials = 0, ID3D11ShaderResourceView **textures = nullptr, int numTextures = 0) : BaseModel(device, _effect, _materials, _numMaterials, textures, numTextures){ init(device, context,width,height, tex_height, tex_normal); };
	ExtendedVertexStruct *vertices;
	UINT*indices;
	//Terrain(UINT widthl, UINT heightl, ID3D11DeviceContext *context, ID3D11Device *device, Effect *_effect,
	//	ID3D11ShaderResourceView *tex_view, Material *_material, ID3D11Texture2D *tex_height, ID3D11Texture2D *tex_normal);

	float CalculateYValue(float x, float z);
	float CalculateYValueWorld(float x, float z);
	void render(ID3D11DeviceContext *context);
	HRESULT init(ID3D11Device *device){ return S_OK; };
	HRESULT init(ID3D11Device *device, ID3D11DeviceContext* context,int _width,int _height, ID3D11Texture2D*tex_height, ID3D11Texture2D*tex_normal);
	~Terrain();


};

