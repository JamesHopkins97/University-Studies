#include "stdafx.h"
#include "Camera.h"
#include <iostream>
Camera::Camera(){};
Camera::Camera(ID3D11Device *device) {
	pos = DirectX::XMVectorSet(0, 0, -10, 1.0f);
	up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	lookAt = DirectX::XMVectorZero();
	initCBuffer(device);
}
Camera::Camera(ID3D11Device *device,DirectX::XMVECTOR init_pos, DirectX::XMVECTOR init_up, DirectX::XMVECTOR init_lookAt) {
	pos = init_pos;
	up = init_up;
	lookAt = init_lookAt;
	initCBuffer(device);
}

void Camera::initCBuffer(ID3D11Device *device){
	cBufferCPU = (CBufferCamera*)_aligned_malloc(sizeof(CBufferCamera), 16);
	cBufferCPU->viewMatrix = getViewMatrix();
	projMatrix = XMMatrixPerspectiveFovLH(0.5f*3.14, 1.0, 0.0001f, 1000.0f);
	cBufferCPU->projMatrix = getProjMatrix();
	XMStoreFloat4(&(cBufferCPU->eyePos), getPos());

	D3D11_BUFFER_DESC cbufferDesc;
	D3D11_SUBRESOURCE_DATA cbufferInitData;
	ZeroMemory(&cbufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&cbufferInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	cbufferDesc.ByteWidth = sizeof(CBufferCamera);
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferInitData.pSysMem = cBufferCPU;
	device->CreateBuffer(&cbufferDesc, &cbufferInitData, &cBufferGPU);
}

Camera::~Camera()
{
}
void Camera::update(ID3D11DeviceContext *context) {
	cBufferCPU->viewMatrix = getViewMatrix();
	cBufferCPU->projMatrix = getProjMatrix();
	XMStoreFloat4(&(cBufferCPU->eyePos), getPos());

	mapCbuffer(context, cBufferCPU, cBufferGPU, sizeof(CBufferCamera));
	context->PSSetConstantBuffers(1, 1, &cBufferGPU);
	context->VSSetConstantBuffers(1, 1, &cBufferGPU);

}

ID3D11Buffer* Camera::getCBuffer() {
	return cBufferGPU;
}

//
// Accessor methods
//
void Camera::setProjMatrix(DirectX::XMMATRIX setProjMat) {
	projMatrix = setProjMat;
}
void Camera::setLookAt(DirectX::XMVECTOR init_lookAt) {
	lookAt = init_lookAt;
}
void Camera::setPos(DirectX::XMVECTOR init_pos) {
	pos = init_pos;
}
void Camera::setUp(DirectX::XMVECTOR init_up) {
	up = init_up;
}
DirectX::XMMATRIX Camera::getViewMatrix() {
	return 		DirectX::XMMatrixLookAtLH(pos, lookAt, up);
}
DirectX::XMMATRIX Camera::getProjMatrix() {
	return 		projMatrix;
}
DirectX::XMVECTOR Camera::getPos() {
	return pos;
}
DirectX::XMVECTOR Camera::getLookAt() {
	return lookAt;
}
DirectX::XMVECTOR Camera::getUp() {
	return up;
}

