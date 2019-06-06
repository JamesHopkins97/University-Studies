
//
// Fire effect 
//

// Ensure matrices are row-major
#pragma pack_matrix(row_major)


//-----------------------------------------------------------------
// Structures and resources
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Globals
//-----------------------------------------------------------------

cbuffer particlesCBuffer : register(b4) {


	float4x4			worldMatrix;
	float				speedFactor; // speed
	float				scaleFactor; // speed
	float				timeOffset; // prevent syncronised particle systems

};
cbuffer cameraCBuffer : register(b1) {
	float4x4			viewMatrix;
	float4x4			projMatrix;
	float4				eyePos;
};

cbuffer sceneCBuffer : register(b3) {
	float4				windDir;
	float				Timer;
	float				grassHeight;
};


//
// Textures
//

// Assumes texture bound to texture t0 and sampler bound to sampler s0
Texture2D flareTexture : register(t0);
Texture2DMS  <float>depth: register(t1);
SamplerState linearSampler : register(s0);




//-----------------------------------------------------------------
// Input / Output structures
//-----------------------------------------------------------------

// Input fragment - this is the per-fragment packet interpolated by the rasteriser stage
struct FragmentInputPacket {

	float4				colour		: COLOR;
	float2				texCoord	:TEXCOORD;
	float4				posH			: SV_POSITION;
};


struct FragmentOutputPacket {

	float4				fragmentColour : SV_TARGET;
};


//-----------------------------------------------------------------
// Pixel Shader - Simple
//-----------------------------------------------------------------

FragmentOutputPacket main(FragmentInputPacket p) {
	
	FragmentOutputPacket outputFragment;

	float g = flareTexture.Sample(linearSampler, p.texCoord).r*0.2;

	outputFragment.fragmentColour = float4(g*p.colour);
	return outputFragment;

}
