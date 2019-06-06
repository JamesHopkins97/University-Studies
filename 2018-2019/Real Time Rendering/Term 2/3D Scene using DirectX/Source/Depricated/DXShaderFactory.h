
//
// DXShaderFactory.h
//

// Provide Compiled Shader Object (CSO) helper functions to load and setup D3D shader interfaces

#pragma once

#include <d3d11_2.h>
#include <string>


// Structure used to describe the SO stage when creating a geometry shader with Stream-out capability
struct DXStreamOutConfig {

	UINT							streamOutSize; // Number of entries in streamOutDeclaration
	D3D11_SO_DECLARATION_ENTRY		*streamOutDeclaration;

	UINT							numVertexStrides; // Number of stride entires in streamOutVertexStrides
	UINT							*streamOutVertexStrides;

	UINT							rasteriseStreamIndex;
};


class DXBlob;


class DXShaderFactory {

public:

	// Load the given vertex shader (CSO) file and return the VS interface in **vertexShaderInterface.  The bytecode interface pointer **vertexShaderBytecode is optional.  If vertexShaderBytecode is given, then ownership of the returned DXBlob is 1 (the caller of loadVertexShader gets implicit ownership).
	static HRESULT loadVertexShader(ID3D11Device *device, const std::string& filepath, ID3D11VertexShader **vertexShaderInterface, DXBlob **vertexShaderBytecode = nullptr, ID3D11ClassLinkage* classLinkage = nullptr);


	// Load the given hull shader (CSO) file and return the HS interface in **hullShaderInterface.  The bytecode interface pointer **hullShaderBytecode is optional.
	static HRESULT loadHullShader(ID3D11Device *device, const std::string& filepath, ID3D11HullShader **hullShaderInterface, DXBlob **hullShaderBytecode = nullptr, ID3D11ClassLinkage* classLinkage = nullptr);


	// Load the given domain shader (CSO) file and return the DS interface in **domainShaderInterface.  The bytecode interface pointer **domainShaderBytecode is optional.
	static HRESULT loadDomainShader(ID3D11Device *device, const std::string& filepath, ID3D11DomainShader **domainShaderInterface, DXBlob **domainShaderBytecode = nullptr, ID3D11ClassLinkage* classLinkage = nullptr);
	

	// Load the given geometry shader (CSO) file and return the GS interface in **geometryShaderInterface.  The bytecode interface pointer **geometryShaderBytecode is optional.
	static HRESULT loadGeometryShader(ID3D11Device *device, const std::string& filepath, DXStreamOutConfig *soConfig, ID3D11GeometryShader **geometryShaderInterface, DXBlob **geometryShaderBytecode = nullptr, ID3D11ClassLinkage* classLinkage = nullptr);
	

	// Load the given pixel shader (CSO) file and return the PS interface in **pixelShaderInterface.  The bytecode interface pointer **pixelShaderBytecode is optional.
	static HRESULT loadPixelShader(ID3D11Device *device, const std::string& filepath, ID3D11PixelShader **pixelShaderInterface, DXBlob **pixelShaderBytecode = nullptr, ID3D11ClassLinkage* classLinkage = nullptr);


	// Load the given compute shader (CSO) file and return the CS interface in **computeShaderInterface.  The bytecode interface pointer **computeShaderBytecode is optional.
	static HRESULT loadComputeShader(ID3D11Device *device, const std::string& filepath, ID3D11ComputeShader **computeShaderInterface, DXBlob **computeShaderBytecode = nullptr, ID3D11ClassLinkage* classLinkage = nullptr);


	// Load the Compiled Shader Object (CSO) file 'filename' and return the bytecode in the blob object **bytecode.  This is used to create shader interfaces that require class linkage interfaces.
	static void loadCSO(const char *filename, DXBlob **bytecode);

};
