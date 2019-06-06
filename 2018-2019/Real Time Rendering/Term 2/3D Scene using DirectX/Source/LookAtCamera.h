#pragma once

#include <DirectXMath.h>
#include <Camera.h>

class LookAtCamera : public Camera 
{

private:

public:

	//
	// Camera transformations
	//
	LookAtCamera(ID3D11Device *device) : Camera(device){};
	LookAtCamera(ID3D11Device *device, DirectX::XMVECTOR init_pos, DirectX::XMVECTOR init_up, DirectX::XMVECTOR init_lookAt) :Camera(device, init_pos, init_up, init_lookAt){};
	
	void rotateElevation(float t) {pos = DirectX::XMVector4Transform(pos, DirectX::XMMatrixRotationX(t));}
	void rotateOnYAxis(float t) {pos = DirectX::XMVector4Transform(pos, DirectX::XMMatrixRotationY(t));}
	void zoomCamera(float zoomFactor) {pos = DirectX::XMVector4Transform(pos, DirectX::XMMatrixScaling(zoomFactor, zoomFactor, zoomFactor));}
};

