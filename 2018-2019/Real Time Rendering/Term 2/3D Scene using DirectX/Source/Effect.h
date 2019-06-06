#pragma once
#include <Utils.h>

class Effect
{
	// Pipeline Input Layout
	ID3D11InputLayout						*VSInputLayout = nullptr;
	
	// Pipeline Shaders
	ID3D11VertexShader						*VertexShader = nullptr;
	ID3D11PixelShader						*PixelShader = nullptr;
	ID3D11GeometryShader					*GeometryShader = nullptr;
	ID3D11HullShader						*HullShader = nullptr;
	ID3D11DomainShader						*DomainShader = nullptr;
	
	// Pipeline States
	ID3D11RasterizerState					*RasterizerState = nullptr;
	ID3D11DepthStencilState					*DepthStencilState = nullptr;
	ID3D11BlendState						*BlendState = nullptr;
	FLOAT			blendFactor[4];
	UINT			sampleMask;
	
public:
	// Setup pipeline for this effect
	void bindPipeline(ID3D11DeviceContext *context);
	
	// Initalise Default Pipeline States
	void initDefaultStates(ID3D11Device *device);
	
	// Assign pre-loaded shaders
	Effect(ID3D11Device *device, ID3D11VertexShader	*_VertexShader, ID3D11PixelShader *_PixelShader, ID3D11InputLayout *_VSInputLayout);
	
	//Load shaders given shader path
	Effect(ID3D11Device *device, const char *vertexShaderPath, const char *pixelShaderPath, const D3D11_INPUT_ELEMENT_DESC vertexDesc[], UINT numVertexElements);

	// Getter and setter methods
	ID3D11InputLayout		*getVSInputLayout(){ return VSInputLayout; };
	ID3D11VertexShader		*getVertexShader(){ return VertexShader; };
	ID3D11PixelShader		*getPixelShader(){ return PixelShader; };
	ID3D11GeometryShader	*getGeometryShader(){ return GeometryShader; };
	ID3D11RasterizerState	*getRasterizerState(){ return RasterizerState; };
	ID3D11DepthStencilState	*getDepthStencilState(){ return DepthStencilState; };
	ID3D11BlendState		*getBlendState(){ return BlendState; };

	void setPixelShader(ID3D11PixelShader	*_PixelShader){ PixelShader = _PixelShader; };
	void setGeometryShader(ID3D11GeometryShader	*_GeometryShader){ GeometryShader = _GeometryShader; };
	void setVertexShader(ID3D11VertexShader	*_VertexShader){ VertexShader = _VertexShader; };
	void setVSInputLayout(ID3D11InputLayout	*_VSInputLayout){ VSInputLayout = _VSInputLayout; };
	void setRasterizerState(ID3D11RasterizerState	*_RasterizerState){ RasterizerState = _RasterizerState; };
	void setDepthStencilState(ID3D11DepthStencilState	*_DepthStencilState){ DepthStencilState = _DepthStencilState; };
	void setBlendState(ID3D11BlendState	*_BlendState){ BlendState = _BlendState; };

	// Shader Creation Wrapper methods
	uint32_t Effect::CreateVertexShader(ID3D11Device *device, const char *filename, char **VSBytecode, ID3D11VertexShader **vertexShader);
	HRESULT Effect::CreatePixelShader(ID3D11Device *device, const char *filename, char **PSBytecode, ID3D11PixelShader **pixelShader);
	HRESULT Effect::CreateGeometryShader(ID3D11Device *device, const char *filename, char **GSBytecode, ID3D11GeometryShader **geometryShader);
	HRESULT Effect::CreateHullShader(ID3D11Device *device, const char *filename, char **GSBytecode, ID3D11HullShader **hullShader);
	HRESULT Effect::CreateDomainShader(ID3D11Device *device, const char *filename, char **GSBytecode, ID3D11DomainShader **domainShader);

	~Effect();
};

