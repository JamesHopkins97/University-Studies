
#include "stdafx.h"
#include <DXModelInstance.h>
#include <DXBaseModel.h>
#include <buffers.h>


using namespace DirectX;


DXModelInstance::DXModelInstance(DXBaseModel *_model, const XMFLOAT3& initT, const XMFLOAT3& initE) {

	model = _model;

	if (model)
		model->retain();

	T = initT;
	E = initE;
}


DXModelInstance::~DXModelInstance() {

	if (model)
		model->release();
}


void DXModelInstance::translate(const XMFLOAT3& dT) {

	T = XMFLOAT3(T.x + dT.x, T.y + dT.y, T.z + dT.z);
}


void DXModelInstance::rotate(const XMFLOAT3& dE) {

	E = XMFLOAT3(E.x + dE.x, E.y + dE.y, E.z + dE.z);
}


void DXModelInstance::setupCBuffer(ID3D11DeviceContext *context, ID3D11Buffer *cbuffer) {

	worldTransformStruct	W;
	XMVECTOR				det;

	W.worldMatrix = XMMatrixRotationRollPitchYaw(E.x, E.y, E.z) * XMMatrixTranslation(T.x, T.y, T.z);
	W.normalMatrix = XMMatrixTranspose(XMMatrixInverse(&det, W.worldMatrix));

	mapBuffer<worldTransformStruct>(context, &W, cbuffer);
}

void DXModelInstance::setupCBuffer(ID3D11DeviceContext *context, ID3D11Buffer *cbuffer, XMFLOAT3& dS) {

	worldTransformStruct	W;
	XMVECTOR				det;

	W.worldMatrix = XMMatrixRotationRollPitchYaw(E.x, E.y, E.z) * XMMatrixTranslation(T.x, T.y, T.z)*XMMatrixScaling(dS.x, dS.y, dS.z);
	W.normalMatrix = XMMatrixTranspose(XMMatrixInverse(&det, W.worldMatrix));

	mapBuffer<worldTransformStruct>(context, &W, cbuffer);
}
void DXModelInstance::render(ID3D11DeviceContext *context) {

	if (model)
		model->render(context);
}
