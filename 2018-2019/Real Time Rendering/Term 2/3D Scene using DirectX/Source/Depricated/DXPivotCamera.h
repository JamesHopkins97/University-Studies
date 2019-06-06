#pragma once

#include <DirectXMath.h>
#include <GUObject.h>

class DXPivotCamera : public GUObject {

private:

	float							cameraXZ, cameraY, cameraR;

public:

	DXPivotCamera() {

		cameraXZ = 0.0f;
		cameraY = 0.0f;
		cameraR = 10.0f;
	}

	DXPivotCamera(float init_cameraXZ, float init_cameraY, float init_cameraR) {

		cameraXZ = init_cameraXZ;
		cameraY = init_cameraY;
		cameraR = init_cameraR;
	}


	//
	// Accessor methods
	//

	float getCameraXZ() {
		
		return cameraXZ;
	}
	
	float getCameraY() {
		
		return cameraY;
	}

	float getCameraR() {
		
		return cameraR;
	}


	//
	// Camera transformations
	//
	
	void rotateElevation(float t) {

		if (fabsf(cameraXZ + t) <= 180.0f)
			cameraXZ += t;
	}

	void rotateOnYAxis(float t) {

		cameraY += t;
	}

	void zoomCamera(float zoomFactor) {

		if (zoomFactor > 0.0f)
			cameraR *= zoomFactor;
	}

	DirectX::XMMATRIX dxViewTransform() {

		DirectX::XMMATRIX rXZ = DirectX::XMMatrixRotationX(cameraXZ);
		DirectX::XMMATRIX rY = DirectX::XMMatrixRotationY(cameraY);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(0.0, 0.0, cameraR);

		DirectX::XMMATRIX R = rY * rXZ * T;

		return R;
	}

	DirectX::XMVECTOR getCameraPos() {

		DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(cameraY) * DirectX::XMMatrixRotationX(cameraXZ);
		DirectX::XMFLOAT4 z;
		DirectX::XMStoreFloat4(&z, R.r[3]);
		DirectX::XMVECTOR v = DirectX::XMVectorSet(z.x*cameraR, z.y*cameraR, -z.z*cameraR, 1.0f);

		DirectX::XMMATRIX rXZ = DirectX::XMMatrixRotationX(cameraXZ);
		DirectX::XMMATRIX rY = DirectX::XMMatrixRotationY(cameraY);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(0.0, 0.0, cameraR);
		v = DirectX::XMVectorSet(0, 0, 1, 0);
		R = T*rXZ*rY;
		v = DirectX::XMVector4Transform(v, R);
		return v;
	}

};

