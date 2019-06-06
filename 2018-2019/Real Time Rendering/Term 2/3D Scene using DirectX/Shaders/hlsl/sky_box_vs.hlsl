// Ensure matrices are row-major
#pragma pack_matrix(row_major)

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
};

//-----------------------------------------------------------------
// Input / Output structures
//-----------------------------------------------------------------
struct vertexInputPacket {
	float3				pos			: POSITION;
	float3				normal		: NORMAL;
	float4				matDiffuse	: DIFFUSE; // a represents alpha.
	float4				matSpecular	: SPECULAR;  // a represents specular power. 
	float2				texCoord	: TEXCOORD;
};

struct vertexOutputPacket {
	float3				texCoord		: TEXCOORD;
	float4				posH			: SV_POSITION;
};


//-----------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------
vertexOutputPacket main(vertexInputPacket inputVertex) {

	float4x4 WVP = mul(worldMatrix, mul(viewMatrix, projMatrix));
	vertexOutputPacket outputVertex;

	// Add Code Here (Output the correct 3D texture coordinates ) 
	outputVertex.texCoord = inputVertex.pos;

	// Add Code Here (ensure  .z is on far clipping plane)
	// Transform/project pos to screen/clip space posH ensuring that pos.z=1(far clipping plane)
	outputVertex.posH = mul(float4(inputVertex.pos, 1.0), WVP).xyww;
	//outputVertex.posH = mul(float4(inputVertex.pos, 1.0), WVP).xyzw;

	return outputVertex;
}
