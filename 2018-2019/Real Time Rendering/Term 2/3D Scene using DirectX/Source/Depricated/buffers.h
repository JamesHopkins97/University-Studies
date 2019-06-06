
//
// buffers.h
//

// Model structures and helper functions to map common data structures to DirectX Constant Buffers (cbuffers) used in different shader implementations.  Structures are 16 byte aligned to match the default HLSL float4 struct alignment.

#pragma once

#include <d3d11_2.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>


// Templated helper function to map cbuffers or 1D resources that are created as dynamic buffers with CPU write access
template <class T>
HRESULT mapBuffer(ID3D11DeviceContext *context, T *srcBuffer, ID3D11Buffer *buffer) {

	D3D11_MAPPED_SUBRESOURCE res;

	HRESULT hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

	if (SUCCEEDED(hr)) {

		memcpy(res.pData, srcBuffer, sizeof(T));
		context->Unmap(buffer, 0);
	}

	return hr;
}


// Templated helper function to create a new dynamic cbuffer
template <class T>
HRESULT createCBuffer(ID3D11Device *device, T *srcBuffer, ID3D11Buffer **cBuffer) {

	D3D11_BUFFER_DESC cbufferDesc;
	D3D11_SUBRESOURCE_DATA cbufferInitData;

	ZeroMemory(&cbufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&cbufferInitData, sizeof(D3D11_SUBRESOURCE_DATA));

	cbufferDesc.ByteWidth = sizeof(T);
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferInitData.pSysMem = srcBuffer;

	HRESULT hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData, cBuffer);

	return hr;
}


// --------------------------------------------------


// Example camera structure
__declspec(align(16)) struct cameraStruct {

	DirectX::XMMATRIX			viewProjMatrix;

	// Position of the camera in World coordinates
	DirectX::XMFLOAT3			eyePos;


	cameraStruct() {

		ZeroMemory(this, sizeof(cameraStruct));

		viewProjMatrix = DirectX::XMMatrixIdentity();
		eyePos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
};


// Example struct that stores only view transform data
__declspec(align(16)) struct cameraPositionStruct {

	DirectX::XMMATRIX					viewMatrix;

	cameraPositionStruct() {

		ZeroMemory(this, sizeof(cameraPositionStruct));
		viewMatrix = DirectX::XMMatrixIdentity();
	}
};


// Example struct used to store the inverse view transform (move from eye/view coordinate space back to world coordinate back)
__declspec(align(16)) struct cameraInverseViewStruct {

	DirectX::XMMATRIX					invViewMatrix;

	cameraInverseViewStruct() {

		ZeroMemory(this, sizeof(cameraInverseViewStruct));
		invViewMatrix = DirectX::XMMatrixIdentity();
	}
};


// Example struct to store projection transform data
__declspec(align(16)) struct cameraProjectionStruct {

	DirectX::XMMATRIX			projMatrix;
	FLOAT						nearPlane;
	FLOAT						farPlane;

	cameraProjectionStruct() {

		ZeroMemory(this, sizeof(cameraProjectionStruct));
		projMatrix = DirectX::XMMatrixIdentity();
		nearPlane = 0.1f;
		farPlane = 1000.0f;
	}
};


// Structure to represent the Model Space to World Space transformation of an object
__declspec(align(16)) struct worldTransformStruct {

	DirectX::XMMATRIX					worldMatrix;
	DirectX::XMMATRIX					normalMatrix;

	worldTransformStruct() {

		worldMatrix = DirectX::XMMatrixIdentity();
		normalMatrix = DirectX::XMMatrixIdentity();
	}
};


// Structure to model the current game time elapsed - this is used to control GPU-based animations
__declspec(align(16)) struct gameTimeStruct {

	FLOAT					gameTime;

	gameTimeStruct() {

		ZeroMemory(this, sizeof(gameTimeStruct));
		gameTime = 0.0f;
	}
};


// Model a single point light source
__declspec(align(16)) struct DXPointLight {

	DirectX::XMFLOAT3		pos;
	FLOAT					range;
	DirectX::XMFLOAT4		ambient;
	DirectX::XMFLOAT4		diffuse;
	DirectX::XMFLOAT4		specular;
	DirectX::XMFLOAT3		attenuation;

	DXPointLight() {

		ZeroMemory(this, sizeof(DXPointLight));
	}

	DXPointLight(
		const DirectX::XMFLOAT3& initPos,
		const FLOAT initRange,
		const DirectX::XMFLOAT4& initAmbient,
		const DirectX::XMFLOAT4& initDiffuse,
		const DirectX::XMFLOAT4& initSpecular,
		const DirectX::XMFLOAT3& initAttenuation) {

		ZeroMemory(this, sizeof(DXPointLight));

		pos = initPos;
		range = initRange;
		ambient = initAmbient;
		diffuse = initDiffuse;
		specular = initSpecular;
		attenuation = initAttenuation;
	}
};
