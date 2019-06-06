
//
// DXShaderFactory.cpp
//

#include <stdafx.h>
#include <DXShaderFactory.h>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <functional>
#include <exception>
#include <DXBlob.h>

using namespace std;


static char* shaderName[] = {

	"Vertex",
	"Hull",
	"Domain",
	"Geometry",
	"Pixel",
	"Compute"
};

static char* shaderProfile[] = { // Depricated

	"vs_5_0",
	"hs_5_0",
	"ds_5_0",
	"gs_5_0",
	"ps_5_0",
	"cs_5_0"
};


template <class T> static int8_t _shaderIndex() { return -1; }
template <> static int8_t _shaderIndex<ID3D11VertexShader>() { return 0; }
template <> static int8_t _shaderIndex<ID3D11HullShader>() { return 1; }
template <> static int8_t _shaderIndex<ID3D11DomainShader>() { return 2; }
template <> static int8_t _shaderIndex<ID3D11GeometryShader>() { return 3; }
template <> static int8_t _shaderIndex<ID3D11PixelShader>() { return 4; }
template <> static int8_t _shaderIndex<ID3D11ComputeShader>() { return 5; }

template <class T> static string _typename() { return string("ID3D11") + shaderName[_shaderIndex<T>()] + string("Shader"); }


//
// Private functions
//

template <class T>
static HRESULT loadShader(
	ID3D11Device *device,
	const string& filepath,
	function<HRESULT(DXBlob*, ID3D11ClassLinkage*, T**)> createShaderInterface,
	T **shaderInterface,
	DXBlob **shaderBytecode,
	ID3D11ClassLinkage* classLinkage)
{
	T			*shader = nullptr;
	DXBlob		*bytecode = nullptr;

	try
	{
		// Load shader bytecode and return the KBLOB in bytecode.  Ownership implicitly passes to loadShader with byteCode->retainCount = 1.
		DXShaderFactory::loadCSO(filepath.c_str(), &bytecode);

		// Create the shader interface
		HRESULT hr = createShaderInterface(bytecode, classLinkage, &shader);

		//hr = E_FAIL; // Test exception handler

		if (!SUCCEEDED(hr)) {

			string errorString(string("Cannot create the ") + _typename<T>() + string(" interface for the hlsl file \"") + filepath + string("\".  Check the ID3D11Device used to create the interface and any shader configuration parameters are valid.\n"));

			throw exception(errorString.c_str());
		}

		// Return the shader bytecode and interface
		*shaderInterface = shader;

		// Pass ownership of bytecode to the caller
		if (shaderBytecode)
			*shaderBytecode = bytecode;
		else
			bytecode->release(); // If not returned, we don't need bytecode so release

		return S_OK;
	}
	catch (exception& e)
	{
		// Report error
		cout << e.what() << endl;

		// Cleanup
		if (shader)
			shader->Release();

		if (bytecode)
			delete bytecode;

		return E_FAIL;
	}
}



//
// Public interface
//

// Load the given vertex shader (CSO) file and return the VS interface in **vertexShaderInterface.  The bytecode interface pointer **vertexShaderBytecode is optional.
HRESULT DXShaderFactory::loadVertexShader(ID3D11Device *device, const std::string& filepath, ID3D11VertexShader **vertexShaderInterface, DXBlob **vertexShaderBytecode, ID3D11ClassLinkage* classLinkage) {

	return loadShader<ID3D11VertexShader>(
		device,
		filepath,
		[=](DXBlob* bytecode, ID3D11ClassLinkage* classLinkage, ID3D11VertexShader** _shaderInterface)->HRESULT {
		return device->CreateVertexShader(bytecode->getBufferPointer(), bytecode->getBufferSize(), classLinkage, _shaderInterface); },
			vertexShaderInterface,
			vertexShaderBytecode,
			classLinkage);
}


// Load the given hull shader (CSO) file and return the HS interface in **hullShaderInterface.  The bytecode interface pointer **hullShaderBytecode is optional.
HRESULT DXShaderFactory::loadHullShader(ID3D11Device *device, const std::string& filepath, ID3D11HullShader **hullShaderInterface, DXBlob **hullShaderBytecode, ID3D11ClassLinkage* classLinkage) {

	return loadShader<ID3D11HullShader>(
		device,
		filepath,
		[=](DXBlob* bytecode, ID3D11ClassLinkage* classLinkage, ID3D11HullShader** _shaderInterface)->HRESULT {
		return device->CreateHullShader(bytecode->getBufferPointer(), bytecode->getBufferSize(), classLinkage, _shaderInterface); },
			hullShaderInterface,
			hullShaderBytecode,
			classLinkage);
}


// Load the given domain shader (CSO) file and return the DS interface in **domainShaderInterface.  The bytecode interface pointer **domainShaderBytecode is optional.
HRESULT DXShaderFactory::loadDomainShader(ID3D11Device *device, const std::string& filepath, ID3D11DomainShader **domainShaderInterface, DXBlob **domainShaderBytecode, ID3D11ClassLinkage* classLinkage) {

	return loadShader<ID3D11DomainShader>(
		device,
		filepath,
		[=](DXBlob* bytecode, ID3D11ClassLinkage* classLinkage, ID3D11DomainShader** _shaderInterface)->HRESULT {
		return device->CreateDomainShader(bytecode->getBufferPointer(), bytecode->getBufferSize(), classLinkage, _shaderInterface); },
			domainShaderInterface,
			domainShaderBytecode,
			classLinkage);
}


// Load the given geometry shader (CSO) file and return the GS interface in **geometryShaderInterface.  The bytecode interface pointer **geometryShaderBytecode is optional.
HRESULT DXShaderFactory::loadGeometryShader(ID3D11Device *device, const std::string& filepath, DXStreamOutConfig *soConfig, ID3D11GeometryShader **geometryShaderInterface, DXBlob **geometryShaderBytecode, ID3D11ClassLinkage* classLinkage) {

	if (soConfig == nullptr) {

		return loadShader<ID3D11GeometryShader>(
			device,
			filepath,
			[=](DXBlob* bytecode, ID3D11ClassLinkage* classLinkage, ID3D11GeometryShader** _shaderInterface)->HRESULT {
			return device->CreateGeometryShader(bytecode->getBufferPointer(), bytecode->getBufferSize(), classLinkage, _shaderInterface); },
				geometryShaderInterface,
				geometryShaderBytecode,
				classLinkage);

	}
	else {

		return loadShader<ID3D11GeometryShader>(
			device,
			filepath,
			[=](DXBlob* bytecode, ID3D11ClassLinkage* classLinkage, ID3D11GeometryShader** _shaderInterface)->HRESULT {
			return device->CreateGeometryShaderWithStreamOutput(
				bytecode->getBufferPointer(),
				bytecode->getBufferSize(),
				soConfig->streamOutDeclaration,
				soConfig->streamOutSize,
				soConfig->streamOutVertexStrides,
				soConfig->numVertexStrides,
				soConfig->rasteriseStreamIndex,
				classLinkage,
				_shaderInterface); },
				geometryShaderInterface,
				geometryShaderBytecode,
				classLinkage);
	}
}


// Load the given pixel shader (CSO) file and return the PS interface in **pixelShaderInterface.  The bytecode interface pointer **pixelShaderBytecode is optional.
HRESULT DXShaderFactory::loadPixelShader(ID3D11Device *device, const std::string& filepath, ID3D11PixelShader **pixelShaderInterface, DXBlob **pixelShaderBytecode, ID3D11ClassLinkage* classLinkage) {

	return loadShader<ID3D11PixelShader>(
		device,
		filepath,
		[=](DXBlob* bytecode, ID3D11ClassLinkage* classLinkage, ID3D11PixelShader** _shaderInterface)->HRESULT {
		return device->CreatePixelShader(bytecode->getBufferPointer(), bytecode->getBufferSize(), classLinkage, _shaderInterface); },
			pixelShaderInterface,
			pixelShaderBytecode,
			classLinkage);
}


// Load the given compute shader (CSO) file and return the CS interface in **computeShaderInterface.  The bytecode interface pointer **computeShaderBytecode is optional.
HRESULT DXShaderFactory::loadComputeShader(ID3D11Device *device, const std::string& filepath, ID3D11ComputeShader **computeShaderInterface, DXBlob **computeShaderBytecode, ID3D11ClassLinkage* classLinkage) {

	return loadShader<ID3D11ComputeShader>(
		device,
		filepath,
		[=](DXBlob* bytecode, ID3D11ClassLinkage* classLinkage, ID3D11ComputeShader** _shaderInterface)->HRESULT {
		return device->CreateComputeShader(bytecode->getBufferPointer(), bytecode->getBufferSize(), classLinkage, _shaderInterface); },
			computeShaderInterface,
			computeShaderBytecode,
			classLinkage);
}



// Load the Compiled Shader Object (CSO) file 'filename' and return the bytecode in the blob object **bytecode.  This is used to create shader interfaces that require class linkage interfaces.
void DXShaderFactory::loadCSO(const char *filename, DXBlob **bytecode) {

	ifstream	*fp = nullptr;
	DXBlob		*memBlock = nullptr;

	try
	{
		// Validate parameters
		if (!filename || !bytecode)
			throw exception("loadCSO: Invalid parameters");

		// Open file
		fp = new ifstream(filename, ios::in | ios::binary);

		if (!fp->is_open())
			throw exception("loadCSO: Cannot open file");

		// Get file size
		fp->seekg(0, ios::end);
		uint32_t size = (uint32_t)fp->tellg();

		// Create blob object to store bytecode (exceptions propagate up if any occur)
		memBlock = new DXBlob(size);

		// Read binary data into blob object
		fp->seekg(0, ios::beg);
		fp->read((char*)(memBlock->getBufferPointer()), memBlock->getBufferSize());


		// Close file and release local resources
		fp->close();
		delete fp;

		// Return DXBlob - ownership implicity passed to caller
		*bytecode = memBlock;
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

		if (memBlock)
			delete memBlock;

		// Re-throw exception
		throw;
	}
}