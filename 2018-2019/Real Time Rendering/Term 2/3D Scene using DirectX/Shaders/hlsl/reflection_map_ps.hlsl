
//
// Model a simple light
//

// Ensure matrices are row-major
#pragma pack_matrix(row_major)


//-----------------------------------------------------------------
// Structures and resources
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Globals
//-----------------------------------------------------------------


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


//
// Textures
//

// Assumes texture bound to texture t0 and sampler bound to sampler s0
Texture2D diffMap : register(t0);
TextureCube envMap : register(t1);
Texture2D specMap : register(t2);

SamplerState linearSampler : register(s0);

//-----------------------------------------------------------------
// Input / Output structures
//-----------------------------------------------------------------

// Input fragment - this is the per-fragment packet interpolated by the rasteriser stage
struct FragmentInputPacket {

	// Vertex in world coords
	float3				posW			: POSITION;
	// Normal in world coords
	float3				normalW			: NORMAL;
	float4				matDiffuse		: DIFFUSE; // a represents alpha.
	float4				matSpecular		: SPECULAR; // a represents specular power. 
	float2				texCoord		: TEXCOORD;
	float4				posH			: SV_POSITION;
};


struct FragmentOutputPacket {

	float4				fragmentColour : SV_TARGET;
};


//-----------------------------------------------------------------
// Pixel Shader - Lighting 
//-----------------------------------------------------------------

FragmentOutputPacket main(FragmentInputPacket v) {

	FragmentOutputPacket outputFragment;

	///////// PARAMETERS Could be added to CBUFFER //////////////////
	float FresnelBias = 0.1;//0.3;
	float FresnelExp = 0.5;//4;
	bool useSpecMap = false;
	bool useDiffMap = false;

	float3 N = normalize(v.normalW);
	float4 baseColour = v.matDiffuse;

	if (useDiffMap)
		baseColour *= diffMap.Sample(linearSampler, v.texCoord);

	//Initialise returned colour to ambient component
	float3 finalColour = baseColour.xyz* lightAmbient;

	// Calculate the lambertian term (essentially the brightness of the surface point based on the dot product of the normal vector with the vector pointing from v to the light source's location)
	float3 lightDir = -lightVec.xyz; // Directional light
	if (lightVec.w == 1.0) lightDir = lightVec.xyz - v.posW; // Positional light
	lightDir = normalize(lightDir);

	// Add diffuse light 
	finalColour += max(dot(lightDir, N), 0.0f) * baseColour.xyz * lightDiffuse;

	// Add reflection
	float specFactor = v.matSpecular.a;

	if (useSpecMap>0.0)
		specFactor *= specMap.Sample(linearSampler, v.texCoord).r;

	float3 eyeDir = normalize(eyePos - v.posW);

	// Add Code Here (Calculate reflection vector ER) 
	float3 ER= float3(0,0,0);
	float3 specColour = specFactor*envMap.Sample(linearSampler, ER).rgb* v.matSpecular.rgb;

	// Calculate Fresnel term
	float facing = 1 - max(dot(N, eyeDir), 0);
	float fres = (FresnelBias + (1.0 - FresnelBias)*pow(abs(facing), abs(FresnelExp)));

	finalColour = (finalColour*(1 - fres)) + (fres * specColour);

	outputFragment.fragmentColour = float4(finalColour, baseColour.a);

	return outputFragment;

}
