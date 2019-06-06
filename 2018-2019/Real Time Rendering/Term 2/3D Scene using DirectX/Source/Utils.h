#pragma once
#include <CBufferStructures.h>

float randM1P1();
HRESULT mapCbuffer(ID3D11DeviceContext *context, void *cBufferExtSrcL, ID3D11Buffer *cBufferExtL,int buffSize);
uint32_t LoadShader(const char *filename, char **bytecode);
