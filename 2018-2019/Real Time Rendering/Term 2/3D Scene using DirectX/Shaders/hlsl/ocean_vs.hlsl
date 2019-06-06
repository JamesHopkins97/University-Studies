
//
// Basic Water Simulation - Modified from NVidia Ocean.fx
//
// NVIDIA Comments :
/****************************************************************************
Simple ocean shader with animated bump map and geometric waves
Based partly on "Effective Water Simulation From Physical Models", GPU Gems
******************************************************************************/

// Ensure matrices are row-major
#pragma pack_matrix(row_major)

//-----------------------------------------------------------------
// Globals
//-----------------------------------------------------------------
#define NWAVES 2

cbuffer modelCBuffer : register(b0) {

	float4x4			worldMatrix;
	float4x4			worldITMatrix; // Correctly transform normals to world space
};
cbuffer cameraCbuffer : register(b1) {
	float4x4			viewMatrix;
	float4x4			projMatrix;
	float4				eyePos;
}
cbuffer lightCBuffer : register(b2) {
	float4				lightVec; // w=1: Vec represents position, w=0: Vec  represents direction.
	float4				lightAmbient;
	float4				lightDiffuse;
	float4				lightSpecular;
};
cbuffer sceneCBuffer : register(b3) {
	float4					windDir;
	float					Time;
	float					grassHeight;
};
//-----------------------------------------------------------------
// Input / Output structures
//-----------------------------------------------------------------
struct vertexInputPacket {

	float3				pos			: POSITION;
	float3				normal		: NORMAL;
	float4				matDiffuse	: DIFFUSE;		// a represents alpha.
	float4				matSpecular	: SPECULAR;		// a represents specular power. 
	float2				texCoord	: TEXCOORD;
};

struct vertexOutputPacket {

	// Eye in world coords
	float3				eyeDir		: POSITION;		// Direction to eye from vertex in world coords
	float3				T2WXf1		: T2WMATRIX1;	// first row of the 3x3 transform from tangent to cube space
	float3				T2WXf2		: T2WMATRIX2;	// second row of the 3x3 transform from tangent to cube space
	float3				T2WXf3		: T2WMATRIX3;	// third row of the 3x3 transform from tangent to cube space
	// Normal map UV  coords
	float2				bumpUV0		: TEXCOORD1;	// Small ripples
	float2				bumpUV1		: TEXCOORD2;	// Medium ripples
	float2				bumpUV2		: TEXCOORD3;	// Large ripples
	float4				posH		: SV_POSITION;  // in clip space
};

// wave structures and functions ///////////////////////
struct Wave {
	float				freq;						// 2*PI / wavelength
	float				amp;						// amplitude
	float				phase;						// speed * 2*PI / wavelength
	float2				dir;
};

float evaluateWave(Wave w, float2 pos, float t)
{
	return w.amp * sin(dot(w.dir, pos)*w.freq + t*w.phase);
}

// derivative of wave function
float evaluateWaveDeriv(Wave w, float2 pos, float t)
{
	return w.freq*w.amp * cos(dot(w.dir, pos)*w.freq + t*w.phase);
}

//-----------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------
vertexOutputPacket main(vertexInputPacket IN) {
	
	float4x4 WVP = mul(worldMatrix, mul(viewMatrix, projMatrix));

	vertexOutputPacket OUT = (vertexOutputPacket)0;
	
	///////// TWEAKABLE PARAMETERS //////////////////
	float				BumpScale = 0.05;
	float				TexReptX = 1;
	float				TexReptY = 1;
	float				BumpSpeedX = -0.005;
	float				BumpSpeedY = 0.005;
	float2				TextureScale = float2(TexReptX, TexReptY);
	float2				BumpSpeed = float2(BumpSpeedX, BumpSpeedY);
	float				WaveAmp = 0.1;
	float				WaveFreq = 1.2;

	Wave wave[NWAVES] = {
		{ WaveFreq, WaveAmp, 0.5, float2(0.0, 0.6) },
		{ WaveFreq * 2, WaveAmp*0.5, 1.3, float2(0.7, 0.0) }
	};

	float4 Po = float4(IN.pos.xyz, 1.0);
	
	// sum waves	
	Po.y = 0.0;
	// Compute y displacement and derivative for the waves defined above
	// Add Code Here (Compute y displacement and derivative)

	
	float ddx = 0.0, ddy = 0.0;
	for (int i = 0; i<NWAVES; i++) {
		Po.y += evaluateWave(wave[i], Po.xz, Time / 2);
		float deriv = evaluateWaveDeriv(wave[i], Po.xz, Time / 2);
		ddx += deriv * wave[i].dir.x;
		ddy += deriv * wave[i].dir.y;
	}

	// compute tangent basis
	float3 B = float3(1, ddx, 0);
	float3 T = float3(0, ddy, 1);
	float3 N = float3(-ddx, 1, -ddy);

	OUT.posH = mul(Po, WVP);

	// pass texture coordinates for fetching the normal map
	float cycle = fmod(Time, 100.0);
	// Modify Code Here (Calculate normal map coordinates - 3 different scales)
	OUT.bumpUV0.xy = 0;
	OUT.bumpUV1.xy = 0;
	OUT.bumpUV2.xy = 0;
	OUT.bumpUV0.xy = IN.texCoord.xy*TextureScale.xy + cycle*BumpSpeed;
	OUT.bumpUV1.xy = IN.texCoord.xy*TextureScale.xy*2.0 + cycle*BumpSpeed*4.0;
	OUT.bumpUV2.xy = IN.texCoord.xy*TextureScale.xy*4.0 + cycle*BumpSpeed*8.0;


	// compute the 3x3 tranform from tangent space to object space
	float3x3 objToTangentSpace;
	// first rows are the tangent and binormal scaled by the bump scale
	objToTangentSpace[0] = BumpScale * normalize(T);
	objToTangentSpace[1] = BumpScale * normalize(B);
	objToTangentSpace[2] = normalize(N);

	OUT.T2WXf1.xyz = mul(objToTangentSpace, worldMatrix[0].xyz);
	OUT.T2WXf2.xyz = mul(objToTangentSpace, worldMatrix[1].xyz);
	OUT.T2WXf3.xyz = mul(objToTangentSpace, worldMatrix[2].xyz);

	// compute the eye vector (going from shaded point to eye) in cube space
	float3 Pw = mul(Po, worldMatrix).xyz;
	OUT.eyeDir = eyePos - Pw; //eye vector

	return OUT;
}