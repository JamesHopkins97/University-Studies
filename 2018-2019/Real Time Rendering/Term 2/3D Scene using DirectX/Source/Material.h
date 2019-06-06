#pragma once
#include <CBufferStructures.h>
using namespace DirectX;
using namespace DirectX::PackedVector;

class 	Effect;
class 	Texture;

class Material
{
	Texture * textures;
	Effect * effects;
	MaterialStruct colour;
public:
	Material();
	~Material();
	MaterialStruct *getColour(){ return &colour; };
	void setEmissive(XMCOLOR _emissive){ colour.emissive = _emissive; };
	void setAmbient(XMCOLOR _ambient){ colour.ambient = _ambient; };
	void setDiffuse(XMCOLOR _diffuse){ colour.diffuse = _diffuse; };
	void setSpecular(XMCOLOR _specular){ colour.specular = _specular; };
	void load();
	void map();
};

