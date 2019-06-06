#include "stdafx.h"
#include "Effect.h"


using namespace std;

void Effect::bindPipeline(ID3D11DeviceContext *context){
	context->RSSetState(RasterizerState);
	// Apply dsState
	context->OMSetDepthStencilState(DepthStencilState, 0);
	context->OMSetBlendState(BlendState, blendFactor, sampleMask);
	context->VSSetShader(VertexShader, 0, 0);
	context->PSSetShader(PixelShader, 0, 0);
	context->GSSetShader(GeometryShader, 0, 0);
	context->DSSetShader(DomainShader, 0, 0);
	context->HSSetShader(HullShader, 0, 0);
}

void Effect::initDefaultStates(ID3D11Device *device ){
	
	// Rasteriser Stage

	// Initialise default Rasteriser state
	D3D11_RASTERIZER_DESC			RSdesc;
	ZeroMemory(&RSdesc, sizeof(D3D11_RASTERIZER_DESC));
	
	// Add code here (Initialise default Rasteriser state)
	// Setup default rasteriser state descriptor

	RSdesc.FillMode = D3D11_FILL_SOLID;
	RSdesc.CullMode = D3D11_CULL_NONE;
	RSdesc.FrontCounterClockwise = FALSE;
	RSdesc.DepthBias = 0;
	RSdesc.SlopeScaledDepthBias = 0.0f;
	RSdesc.DepthBiasClamp = 0.0f;
	RSdesc.DepthClipEnable = TRUE;
	RSdesc.ScissorEnable = FALSE;
	RSdesc.MultisampleEnable = FALSE;
	RSdesc.AntialiasedLineEnable = FALSE;
	// Setup default rasteriser state

	// Create Rasterizer State (RasterizerState) object based on the given descriptor
	HRESULT hr = device->CreateRasterizerState(&RSdesc, &RasterizerState);


	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create Rasterise state interface");



	// Output - Merger Stage

	// Initialise default depth-stencil state object
	D3D11_DEPTH_STENCIL_DESC	dsDesc;
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	// Add code here (Initialise default depth-stencil state)
	// Setup default depth-stencil descriptor


	// Setup default depth-stencil descriptor
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;


	
	// Create depth-stencil state object (DepthStencilState) based on the given descriptor
	hr = device->CreateDepthStencilState(&dsDesc, &DepthStencilState);


	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create DepthStencil state interface");

	// Initialise default blend state object (Alpha Blending On)
	D3D11_BLEND_DESC	blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

	// Setup default blend state descriptor 
	blendDesc.AlphaToCoverageEnable = FALSE; // Use pixel coverage info from rasteriser (default FALSE)
	blendDesc.IndependentBlendEnable = FALSE; // The following array of render target blend properties uses the blend properties from RenderTarget[0] for ALL render targets
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Create blendState object (BlendState) based on the given descriptor
	hr = device->CreateBlendState(&blendDesc, &BlendState);
	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create Blend state interface");

	blendFactor[0] = blendFactor[1] = blendFactor[2] = blendFactor[3] = 1.0f;
	sampleMask = 0xFFFFFFFF; // Bitwise flags to determine which samples to process in an MSAA context
}
Effect::Effect(ID3D11Device *device, ID3D11VertexShader	*_VertexShader, ID3D11PixelShader *_PixelShader, ID3D11InputLayout *_VSInputLayout)
{
	VertexShader = _VertexShader;
	PixelShader = _PixelShader;
	GeometryShader = NULL;
	VSInputLayout = _VSInputLayout;
	initDefaultStates(device);
	VertexShader->AddRef();
	PixelShader->AddRef();
	VSInputLayout->AddRef();

}

Effect::Effect(ID3D11Device *device, const char *vertexShaderPath, const char * pixelShaderPath, const D3D11_INPUT_ELEMENT_DESC vertexDesc[], UINT numVertexElements)
{
	char *tmpShaderBytecode = nullptr;

	uint32_t tmpVSSizeBytes = CreateVertexShader(device, vertexShaderPath, &tmpShaderBytecode, &VertexShader);

	// Add code here (Create an input layout)
	// Create an input layout object(VSInputLayout)


	device->CreateInputLayout(vertexDesc, numVertexElements, tmpShaderBytecode, tmpVSSizeBytes, &VSInputLayout);


	CreatePixelShader(device, pixelShaderPath, &tmpShaderBytecode, &PixelShader);
	initDefaultStates(device);
}




Effect::~Effect()
{
	if (RasterizerState)
		RasterizerState->Release();
	if (DepthStencilState)
		DepthStencilState->Release();
	if (BlendState)
		BlendState->Release();
	if (VertexShader)
		VertexShader->Release();
	if (PixelShader)
		PixelShader->Release();
	if (VSInputLayout)
		VSInputLayout->Release();
}

uint32_t Effect::CreateVertexShader(ID3D11Device *device, const char *filename, char **VSBytecode, ID3D11VertexShader **vertexShader){

	cout << "Loading Vertex Shader" << endl;
	// Add code here (Load compiled Vertex Shader bytecode and create the "ID3D11VertexShader" object)
	
	//Load the compiled vertex shader byte code.

	uint32_t shaderBytes = LoadShader(filename, VSBytecode);
	// Create the "ID3D11VertexShader" object

	HRESULT hr = device->CreateVertexShader(*VSBytecode, shaderBytes, NULL, vertexShader);


	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create VertexShader interface");
	return shaderBytes;
}

HRESULT Effect::CreatePixelShader(ID3D11Device *device, const char *filename, char **PSBytecode, ID3D11PixelShader **pixelShader)
{
	// Initialise programmable pipeline stages – Pixel Shader
	cout << "Loading Vertex Pixel" << endl;

	//Load the compiled pixel shader byte code.
	uint32_t shaderBytes = LoadShader(filename, PSBytecode);

	// Create the "ID3D11PixelShader" object
	HRESULT hr = device->CreatePixelShader(*PSBytecode, shaderBytes, NULL, pixelShader);

	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create PixelShader interface");
	return hr;
}
HRESULT Effect::CreateGeometryShader(ID3D11Device *device, const char *filename, char **GSBytecode, ID3D11GeometryShader **geometryShader)
{
	//char *GSBytecodeLocal = nullptr;
	//GSBytecode = &GSBytecodeLocal;
	cout << "Loading Geometry Shader" << endl;

	//Load the compiled shader byte code.
	uint32_t shaderBytes = LoadShader(filename, GSBytecode);

	cout << "Done: GShader memory bytes = " << shaderBytes << endl;
	// Create shader objects
	HRESULT hr = device->CreateGeometryShader(*GSBytecode, shaderBytes, NULL, geometryShader);
	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create GeometryShader interface");
	return hr;
}

HRESULT Effect::CreateHullShader(ID3D11Device *device, const char *filename, char **HSBytecode, ID3D11HullShader **hullShader)
{
	//char *HSBytecodeLocal = nullptr;
	//HSBytecode = &HSBytecodeLocal;

	cout << "Loading Hull Shader" << endl;

	//Load the compiled shader byte code.
	uint32_t shaderBytes = LoadShader(filename, HSBytecode);

	// Create shader objects
	HRESULT hr = device->CreateHullShader(*HSBytecode, shaderBytes, NULL, hullShader);
	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create hullShader interface");
	return hr;
}

HRESULT Effect::CreateDomainShader(ID3D11Device *device, const char *filename, char **DSBytecode, ID3D11DomainShader **domainShader)
{
	cout << "Loading Domain Shader" << endl;
	//Load the compiled shader byte code.
	uint32_t shaderBytes = LoadShader(filename, DSBytecode);

	// Create shader objects
	HRESULT hr = device->CreateDomainShader(*DSBytecode, shaderBytes, NULL, domainShader);
	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create hullShader interface");
	return hr;
}
