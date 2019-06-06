
#include "stdafx.h"
#include <BaseModel.h>


BaseModel::~BaseModel() {

	if (vertexBuffer)
		vertexBuffer->Release();

	if (indexBuffer)
		indexBuffer->Release();

	if (inputLayout)
		inputLayout->Release();
}
