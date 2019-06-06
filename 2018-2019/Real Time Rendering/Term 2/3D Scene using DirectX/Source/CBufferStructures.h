#pragma once
using namespace DirectX;
using namespace DirectX::PackedVector;
// CBuffer struct
// Use 16byte aligned so can use optimised XMMathFunctions instead of setting _XM_NO_INNTRINSICS_ define when compiling for x86

//// CBuffer struct
//__declspec(align(16)) struct CBufferBasic {
//	DirectX::XMMATRIX		WVPMatrix;
//	DirectX::XMMATRIX		worldITMatrix;
//	DirectX::XMFLOAT3		lightDir;
//};


// CBufferScene contains parameters of a scene
__declspec(align(16)) struct CBufferScene {
	DirectX::XMFLOAT4						windDir;
	FLOAT									Time;
	FLOAT									grassHeight;
};



// CBuffer struct
__declspec(align(16)) struct CBufferBasic {
	DirectX::XMMATRIX		WVPMatrix;
	DirectX::XMMATRIX		worldITMatrix;
	DirectX::XMMATRIX		worldMatrix;
	DirectX::XMFLOAT4		eyePos;
};


__declspec(align(16)) struct CBufferExt  {
	DirectX::XMMATRIX						WVPMatrix;
	DirectX::XMMATRIX						worldITMatrix; // Correctly transform normals to world space
	DirectX::XMMATRIX						worldMatrix;
	DirectX::XMFLOAT4						eyePos;
	DirectX::XMFLOAT4						lightVec; // w=1: Vec represents position, w=0: Vec  represents direction.
	DirectX::XMFLOAT4						lightAmbient;
	DirectX::XMFLOAT4						lightDiffuse;
	DirectX::XMFLOAT4						lightSpecular;

	// from terrain tutorial
	DirectX::XMFLOAT4						windDir;
	// Simple single light source properties
	FLOAT									Timer;
	// from terrain tutorial
	FLOAT									grassHeight;
	//// from terrain tutorial
	FLOAT									reflectionPass;
	// from particle system tutorial
	//FLOAT									fireHeight;
};


__declspec(align(16)) struct CBufferCamera {

	DirectX::XMMATRIX						viewMatrix; // Correctly transform normals to world space
	DirectX::XMMATRIX						projMatrix;
	DirectX::XMFLOAT4						eyePos;
};
__declspec(align(16)) struct CBufferModel {
//	DirectX::XMMATRIX						WVPMatrix;
	DirectX::XMMATRIX						worldMatrix;
	DirectX::XMMATRIX						worldITMatrix; // Correctly transform normals to world space
	//FLOAT									USE_SHADOW_MAP=1; 
};
__declspec(align(16)) struct CBufferShadow {
	DirectX::XMMATRIX						shadowTransformMatrix;
};

__declspec(align(16)) struct CBufferParticles {
	DirectX::XMMATRIX						worldMatrix;
	FLOAT									speedFactor; // speed
	FLOAT									scaleFactor; // scale
	FLOAT									timeOffset; // prevent syncronised particle systems
};
__declspec(align(16)) struct CBufferLight {
	DirectX::XMFLOAT4						lightVec; // w=1: Vec represents position, w=0: Vec  represents direction.
	DirectX::XMFLOAT4						lightAmbient;
	DirectX::XMFLOAT4						lightDiffuse;
	DirectX::XMFLOAT4						lightSpecular;
};


__declspec(align(16)) struct projMatrixStruct  {
	DirectX::XMMATRIX						projMatrix;
};
// from terrain tutorial
__declspec(align(16)) struct worldMatrixStruct  {
	DirectX::XMMATRIX						worldMatrix;
};

struct MaterialStruct
{
	XMCOLOR emissive;
	XMCOLOR ambient;
	XMCOLOR diffuse;
	XMCOLOR specular;
};
struct LightStruct
{
	XMFLOAT4 attenuation; // Constant, Linear, Quadratic Attenuation
	XMFLOAT4 vector; //w = 1: Vec represents position, w = 0 : Vec  represents direction.
	XMCOLOR ambient;
	XMCOLOR diffuse;
	XMCOLOR specular;
};





