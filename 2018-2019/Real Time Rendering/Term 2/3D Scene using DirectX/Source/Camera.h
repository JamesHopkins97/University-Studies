#pragma once
#include <DirectXMath.h>
#include<CBufferStructures.h>
#include<Utils.h>
class Camera
{
protected:
	DirectX::XMVECTOR pos;
	DirectX::XMVECTOR up;
	DirectX::XMVECTOR lookAt;
	DirectX::XMMATRIX projMatrix;
	ID3D11Buffer					*cBufferGPU = nullptr;
	CBufferCamera					*cBufferCPU = nullptr;
	void Camera::initCBuffer(ID3D11Device *device);
public:
	Camera();
	Camera(ID3D11Device *device);
	Camera(ID3D11Device *device, DirectX::XMVECTOR init_pos, DirectX::XMVECTOR init_up, DirectX::XMVECTOR init_lookAt);
	~Camera();

	// Accessor methods
	void setProjMatrix(DirectX::XMMATRIX setProjMat);
	void setLookAt(DirectX::XMVECTOR init_lookAt);
	void setPos(DirectX::XMVECTOR init_pos);
	void setUp(DirectX::XMVECTOR init_up);
	DirectX::XMMATRIX getViewMatrix();
	DirectX::XMMATRIX getProjMatrix();
	DirectX::XMVECTOR getPos();
	DirectX::XMVECTOR getLookAt();
	DirectX::XMVECTOR getUp();
	ID3D11Buffer* getCBuffer();




	void update(ID3D11DeviceContext *context);
};

