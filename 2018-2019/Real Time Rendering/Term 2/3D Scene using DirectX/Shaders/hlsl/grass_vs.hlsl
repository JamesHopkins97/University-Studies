
//
// Grass effect - Modified a fur technique
//

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
struct vertexInputPacket {

	float3				pos			: POSITION;
	float3				normal		: NORMAL;
	float4				matDiffuse	: DIFFUSE; // a represents alpha.
	float4				matSpecular	: SPECULAR;  // a represents specular power. 
	float2				texCoord	: TEXCOORD;
};


struct vertexOutputPacket {


	// Vertex in world coords
	float3				posW			: POSITION;
	// Normal in world coords
	float3				normalW			: NORMAL;
	float4				matDiffuse		: DIFFUSE;
	float4				matSpecular		: SPECULAR;
	float2				texCoord		: TEXCOORD;
	float4				posH			: SV_POSITION;
};
//-----------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------
vertexOutputPacket main(vertexInputPacket inputVertex) {
	float grassScaleFactor = 1.0;
	vertexOutputPacket outputVertex;
	float4x4 WVP = mul(worldMatrix, mul(viewMatrix, projMatrix));
	// Lighting is calculated in world space.
	outputVertex.posW = mul(float4(inputVertex.pos, 1.0f), worldMatrix).xyz;
	// Transform normals to world space with gWorldIT.
	outputVertex.normalW = mul(float4(inputVertex.normal, 1.0f), worldITMatrix).xyz;
	// Pass through material properties
	outputVertex.matDiffuse = inputVertex.matDiffuse;
	outputVertex.matSpecular = inputVertex.matSpecular;
	// .. and texture coordinates.
	outputVertex.texCoord = inputVertex.texCoord;
	// Finally transform/project pos to screen/clip space posH
	float3 pos = inputVertex.pos;
		pos.y += grassHeight*grassScaleFactor;

	float k = pow(grassHeight*100, 3);
	float3 gWindDir = float3(sin(Time)*0.01, 0, 0);
		pos = pos + gWindDir*k;
	outputVertex.posH = mul(float4(pos, 1.0), WVP);

	return outputVertex;
}
