
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
// Structures and resources
//-----------------------------------------------------------------

// Textures

// Assumes normal map bound to texture t0 
Texture2D gNormMap : register(t0);
// Cube map bound to texture t1
TextureCube gCubeMap : register(t1);
// Normal linear sampler bound to sampler s0
//TextureCube gCubeMapDynamic : register(t2);
// Normal linear sampler bound to sampler s0
SamplerState gNormalLinearSam : register(s0);
// Tri-linear sampler bound to sampler s1
SamplerState gTriLinearSam : register(s1);

// Globals

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
	float4						windDir;
	float						Time;
	float						grassHeight;
};

//-----------------------------------------------------------------
// Input / Output structures
//-----------------------------------------------------------------

// Input fragment - this is the per-fragment packet interpolated by the rasteriser stage
struct FragmentInputPacket {
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

struct FragmentOutputPacket {

	float4				fragmentColour : SV_TARGET;
};

//-----------------------------------------------------------------
// Pixel Shader - Lighting 
//-----------------------------------------------------------------

FragmentOutputPacket main(FragmentInputPacket IN) { 

	FragmentOutputPacket outputFragment;
	
	///////// TWEAKABLE PARAMETERS //////////////////
	float FresnelBias =		0.3;
	float FresnelExp =		4.0;
	float3 DeepColor =		 { 0.0f, 0.0f, 0.1f };
	float3 ShallowColor =	{ 0.0f, 0.2f, 0.2f };
	float3 ReflTint =		{ 1.0f, 1.0f, 1.0f };
	float Kr =				1.0f;
	float KWater =			1.0f;
	float HDRMultiplier =	0;


	// Sum normal maps
	float4 t0 = gNormMap.Sample(gNormalLinearSam, IN.bumpUV0)*2.0 - 1.0;
	float4 t1 = gNormMap.Sample(gNormalLinearSam, IN.bumpUV1)*2.0 - 1.0;
	float4 t2 = gNormMap.Sample(gNormalLinearSam, IN.bumpUV2)*2.0 - 1.0;
	float3 Nt = t0.xyz + t1.xyz + t2.xyz;

	// Transform normals from texture space to world coordinates
	// Add Code Here (Reconstruct float3X3 tangent to world matrix)
	float3x3 m; // Tangent to world matrix
	m[0] = IN.T2WXf1;
	m[1] = IN.T2WXf2;
	m[2] = IN.T2WXf3;

	// Transform normal to world coordinates
	// Modify Code Here (Transform normal to world coordinates)
	float3 Nw = mul(m, Nt);
	float3 Nn = normalize(Nw);
	//Nn = float3(0, 1, 0); used for debugging

	// Reflection
	float3 Vn = normalize(IN.eyeDir);
	float3 R = reflect(-Vn, Nn);
	float4 reflection = gCubeMap.Sample(gTriLinearSam, R);
	//reflection.rgb *= (1.0 + reflection.a*HDRMultiplier); //notused
	
	//// Fresnel
	//float facing = 1.0 - max(dot(Vn, Nn), 0);
	//float3 waterColor = KWater * lerp(DeepColor, ShallowColor, facing);
	//// Modify Code Here (Complete Fresnel calculation)
	//float fres = Kr*(FresnelBias + (1.0 - FresnelBias)*pow(facing, FresnelExp));

	//float3 result = waterColor + (fres * reflection.rgb * ReflTint);

	//outputFragment.fragmentColour = float4(result.rgb,  fres);
	//return outputFragment;


	// Fresnel
	float facing = 1.0 - max(dot(Vn, Nn), 0);
	float fres = Kr * (FresnelBias + (1.0 - FresnelBias) 	*pow(abs(facing), abs(FresnelExp)));
	float3 waterColor = KWater * lerp(DeepColor, ShallowColor, facing);
	float3 result = (waterColor*(1 - fres)) + (fres * 	reflection.rgb)* ReflTint;

	outputFragment.fragmentColour = float4(result.rgb, 1.0);

	return outputFragment;

}