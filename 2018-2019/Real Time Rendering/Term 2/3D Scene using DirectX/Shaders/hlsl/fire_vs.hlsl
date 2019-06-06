
//
// Fire effect
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
	float4				windDir;
	float				Time;
	float				grassHeight;
};


//-----------------------------------------------------------------
// Input / Output structures
//-----------------------------------------------------------------
struct vertexInputPacket {

	float3 pos : POSITION;   // in object space
	float3 posL : LPOS;   // in object space
	float3 vel :VELOCITY;   // in object space
	float3 data : DATA;
};


struct vertexOutputPacket {

	float4 posH  : SV_POSITION;  // in clip space
	float2 texCoord  : TEXCOORD0;
	float alpha : ALPHA;
};
//-----------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------
vertexOutputPacket main(vertexInputPacket vin) {
	float4x4 VP = mul(viewMatrix, projMatrix);

	float gPartLife = 0.7;//seconds
	float gPartScale = 0.2;

	float2x2 rotScaleMatrix;
	rotScaleMatrix[0] = worldMatrix[0].xy;
	rotScaleMatrix[1] = worldMatrix[1].xy;
	float2 posL = mul(vin.posL.xy, rotScaleMatrix);

	vertexOutputPacket vout = (vertexOutputPacket)0;

	float age = vin.data.x;
	float ptime = fmod(Time + (age*gPartLife), gPartLife);
	float size = ((gPartScale*ptime) + (gPartScale * 2));
	vout.alpha = 1.0 - (ptime / gPartLife);

	float3 pos = vin.pos+ ptime*vin.vel;
	pos = mul(float4(pos, 1.0), worldMatrix).xyz;

	// Compute camera ortho normal basis to direct billboard faces towards the camera.
	// Add Code Here (Compute ortho normal basis)
	float3 look = normalize(eyePos - pos);
	float3 right = normalize(cross(float3(0, 1, 0), look));
	float3 up = cross(look, right);

	// Transform to world space.
	// Add Code Here (Transform particle verticies to face the camera)
	pos= pos + (posL.x*right*size) + (posL.y*up*size);

	// Transform to homogeneous clip space.
	vout.posH = mul(float4(pos, 1.0f), VP);

	//calculate texture coordinates
	vout.texCoord = float2((vin.posL.x + 1)*0.5, (vin.posL.y + 1)*0.5);
	return vout;

}
