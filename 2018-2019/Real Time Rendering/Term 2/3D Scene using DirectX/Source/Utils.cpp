#include "stdafx.h"
#include "Utils.h"

using namespace std;
// Helper function to copy cbuffer data from cpu to gpu
HRESULT mapCbuffer(ID3D11DeviceContext *context,void *cBufferCPU, ID3D11Buffer *cBufferGPU,int buffSize)
{
	//ID3D11DeviceContext *context = system->getDeviceContext();
	// Map cBuffer
	D3D11_MAPPED_SUBRESOURCE res;
	HRESULT hr = context->Map(cBufferGPU, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

	if (SUCCEEDED(hr)) {
		memcpy(res.pData, cBufferCPU, buffSize);
		context->Unmap(cBufferGPU, 0);
	}
	return hr;
}

// from terrain tutorial
// Helper Generates random number between -1.0 and +1.0
float randM1P1()
{	// use srand((unsigned int)time(NULL)); to seed rand()
	float r = (float)((double)rand() / (double)(RAND_MAX)) *2.0f - 1.0f;

	//modified to return a ring with inner radius A and outer radius A+B
	float A = 1, B = 3;
	r *= B; if (r > 0)r += A; else r -= A;
	return r / 6;
}

// Helper to load a compiled shader
uint32_t LoadShader(const char *filename, char **bytecode)
{

	ifstream	*fp = nullptr;
	uint32_t shaderBytes = -1;
	cout << "loading shader" << endl;

	try
	{
		// Validate parameters
		if (!filename)
			throw exception("loadCSO: Invalid parameters");

		// Open file
		fp = new ifstream(filename, ios::in | ios::binary);

		if (!fp->is_open())
			throw exception("loadCSO: Cannot open file");

		// Get file size
		fp->seekg(0, ios::end);
		shaderBytes = (uint32_t)fp->tellg();

		// Create blob object to store bytecode (exceptions propagate up if any occur)
		//memBlock = new DXBlob(size);
		cout << "allocating shader memory bytes = " << shaderBytes << endl;
		*bytecode = (char*)malloc(shaderBytes);
		// Read binary data into blob object
		fp->seekg(0, ios::beg);
		fp->read(*bytecode, shaderBytes);

		// Close file and release local resources
		fp->close();
		delete fp;

		// Return DXBlob - ownership implicity passed to caller
		//*bytecode = memBlock;
		cout << "Done: shader memory bytes = " << shaderBytes << endl;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;

		// Cleanup local resources
		if (fp) {

			if (fp->is_open())
				fp->close();

			delete fp;
		}

		if (bytecode)
			delete bytecode;

		// Re-throw exception
		throw;
	}
	return shaderBytes;
}

