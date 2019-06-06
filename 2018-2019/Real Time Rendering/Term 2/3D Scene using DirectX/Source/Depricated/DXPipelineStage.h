
#pragma once

#include <d3d11_2.h>
#include <DirectXMath.h>
#include <GUObject.h>
#include <DXBlob.h>
#include <functional>
#include <iostream>
#include <exception>
#include <vector>
#include <DXViewHost.h>
#include <DXShaderFactory.h>


// Pipeline stage interface
class DXPipelineStage: public GUObject {

public:

	virtual void apply(ID3D11DeviceContext *context) = 0;
};



// Generic stage base class for encapsulating shader interfaces
template <class T>
class DXShaderStage : public DXPipelineStage {

protected:

	T						*shaderInterface = nullptr;
	DXBlob					*bytecode = nullptr;

	UINT					numClassInstances = 0;
	ID3D11ClassInstance		**classInstances = nullptr;
	

public:

	DXShaderStage(T *_shaderInterface) {

		if (_shaderInterface) {

			shaderInterface = _shaderInterface;
			shaderInterface->AddRef();
		}
	}

	DXShaderStage(std::function<HRESULT(T**, DXBlob**)> fn) {

		try {

			HRESULT hr = fn(&shaderInterface, &bytecode);

			if (!SUCCEEDED(hr))
				throw std::exception("Cannot load shader - empty pipeline stage setup");
		}
		catch (std::exception& e) {

			std::cout << e.what() << std::endl;

			if (shaderInterface) {

				shaderInterface->Release();
				shaderInterface = nullptr;
			}
			
			if (bytecode) {

				bytecode->release();
				bytecode = nullptr;
			}

			// Re-throw exception
			throw;
		}
	}

	~DXShaderStage() {

		if (shaderInterface)
			shaderInterface->Release();

		if (bytecode)
			bytecode->release();

		if (classInstances) {

			for (UINT i = 0; i < numClassInstances; ++i) {
				
				classInstances[i]->Release();
			}

			free(classInstances);
		}
	}


	T *getInterface() { return shaderInterface; }


	DXBlob *getBytecode() { return bytecode; }

	
	// Create a class instance list and return this to the caller to be populated.  The pipeline stage assumes ownership of the interfaces added to the list by the caller.
	ID3D11ClassInstance** createClassInstanceArray(UINT _numInstances) {

		classInstances = (ID3D11ClassInstance**)calloc(_numInstances, sizeof(ID3D11ClassInstance*));

		if (classInstances)
			numClassInstances = _numInstances;

		return classInstances;
	}
};




// Vertex shader stage
class DXVertexShaderStage : public DXShaderStage<ID3D11VertexShader> {

public:

	DXVertexShaderStage(ID3D11VertexShader *_shaderInterface) : DXShaderStage<ID3D11VertexShader>(_shaderInterface) {}

	// Constructor to initialise the VS shader stage object and return the shader bytecode to the caller if requested.  This is unique to the VS stage because we usually want the bytecode for the vertex shader in order to setup the IA.  Only a weak reference is returned - ownership of the bytecode stays with 'this' object.
	DXVertexShaderStage(ID3D11Device *device, const std::string& filename, DXBlob **_bytecode = nullptr, ID3D11ClassLinkage *classLinkage = nullptr) : DXShaderStage<ID3D11VertexShader>(
		[&](ID3D11VertexShader** shaderInterface, DXBlob** bytecode) -> HRESULT { return DXShaderFactory::loadVertexShader(device, filename, shaderInterface, bytecode, classLinkage); }) {
	
		// Return bytecode if handle passed in.  Ownership stays with DXShaderStage - the caller can retain the bytecode object if required.
		if (_bytecode)
			*_bytecode = bytecode;
	}

	void apply(ID3D11DeviceContext *context) {

		context->VSSetShader(shaderInterface, classInstances, numClassInstances);
	}
};


// Hull shader stage
class DXHullShaderStage : public DXShaderStage<ID3D11HullShader> {

public:

	DXHullShaderStage(ID3D11HullShader *_shaderInterface) : DXShaderStage<ID3D11HullShader>(_shaderInterface) {}

	DXHullShaderStage(ID3D11Device *device, const std::string& filename, ID3D11ClassLinkage *classLinkage = nullptr) : DXShaderStage<ID3D11HullShader>(
		[&](ID3D11HullShader** shaderInterface, DXBlob** bytecode) -> HRESULT { return DXShaderFactory::loadHullShader(device, filename, shaderInterface, bytecode, classLinkage); }) {}

	void apply(ID3D11DeviceContext *context) {

		context->HSSetShader(shaderInterface, classInstances, numClassInstances);
	}
};


// Domain shader stage
class DXDomainShaderStage : public DXShaderStage<ID3D11DomainShader> {

public:

	DXDomainShaderStage(ID3D11DomainShader *_shaderInterface) : DXShaderStage<ID3D11DomainShader>(_shaderInterface) {}

	DXDomainShaderStage(ID3D11Device *device, const std::string& filename, ID3D11ClassLinkage *classLinkage = nullptr) : DXShaderStage<ID3D11DomainShader>(
		[&](ID3D11DomainShader** shaderInterface, DXBlob** bytecode) -> HRESULT { return DXShaderFactory::loadDomainShader(device, filename, shaderInterface, bytecode, classLinkage); }) {}

	void apply(ID3D11DeviceContext *context) {

		context->DSSetShader(shaderInterface, classInstances, numClassInstances);
	}
};


// Geometry shader stage
class DXGeometryShaderStage : public DXShaderStage<ID3D11GeometryShader> {

public:

	DXGeometryShaderStage(ID3D11GeometryShader *_shaderInterface) : DXShaderStage<ID3D11GeometryShader>(_shaderInterface) {}

	DXGeometryShaderStage(ID3D11Device *device, const std::string& filename, DXStreamOutConfig *soConfig = nullptr, ID3D11ClassLinkage *classLinkage = nullptr) : DXShaderStage<ID3D11GeometryShader>(
		[&](ID3D11GeometryShader** shaderInterface, DXBlob** bytecode) -> HRESULT { return DXShaderFactory::loadGeometryShader(device, filename, soConfig, shaderInterface, bytecode, classLinkage); }) {}

	void apply(ID3D11DeviceContext *context) {

		context->GSSetShader(shaderInterface, classInstances, numClassInstances);
	}
};


// Pixel shader stage
class DXPixelShaderStage : public DXShaderStage<ID3D11PixelShader> {

public:

	DXPixelShaderStage(ID3D11PixelShader *_shaderInterface) : DXShaderStage<ID3D11PixelShader>(_shaderInterface) {}

	DXPixelShaderStage(ID3D11Device *device, const std::string& filename, ID3D11ClassLinkage *classLinkage = nullptr) : DXShaderStage<ID3D11PixelShader>(
		[&](ID3D11PixelShader** shaderInterface, DXBlob** bytecode) -> HRESULT { return DXShaderFactory::loadPixelShader(device, filename, shaderInterface, bytecode, classLinkage); }) {}

	void apply(ID3D11DeviceContext *context) {

		context->PSSetShader(shaderInterface, classInstances, numClassInstances);
	}
};



// Classes to model fixed-function stages of the pipeline.  These are fine-grained classes to control specific aspects of the pipeline.

// Rasteriser state object
class DXRasteriserState : public DXPipelineStage {

	D3D11_RASTERIZER_DESC			RSdesc;
	ID3D11RasterizerState			*RSstate = nullptr;

public:

	// Initialise default Rasteriser state object
	DXRasteriserState(ID3D11Device *device) {

		ZeroMemory(&RSdesc, sizeof(D3D11_RASTERIZER_DESC));

		// Setup default rasteriser state
		RSdesc.FillMode = D3D11_FILL_SOLID;
		RSdesc.CullMode = D3D11_CULL_BACK;
		RSdesc.FrontCounterClockwise = FALSE;
		RSdesc.DepthBias = 0;
		RSdesc.SlopeScaledDepthBias = 0.0f;
		RSdesc.DepthBiasClamp = 0.0f;
		RSdesc.DepthClipEnable = TRUE;
		RSdesc.ScissorEnable = FALSE;
		RSdesc.MultisampleEnable = FALSE;
		RSdesc.AntialiasedLineEnable = FALSE;

		HRESULT hr = device->CreateRasterizerState(&RSdesc, &RSstate);

		if (!SUCCEEDED(hr))
			throw std::exception("Cannot create Rasteriser state interface");
	}

	// Initialise Rasteriser state object based on the given descriptor
	DXRasteriserState(ID3D11Device *device, const D3D11_RASTERIZER_DESC& desc) {

		ZeroMemory(&RSdesc, sizeof(D3D11_RASTERIZER_DESC));
		memcpy_s(&RSdesc, sizeof(D3D11_RASTERIZER_DESC), &desc, sizeof(D3D11_RASTERIZER_DESC));

		HRESULT hr = device->CreateRasterizerState(&RSdesc, &RSstate);

		if (!SUCCEEDED(hr))
			throw std::exception("Cannot create Rasteriser state interface");
	}

	~DXRasteriserState() {

		if (RSstate)
			RSstate->Release();
	}

	void apply(ID3D11DeviceContext *context) {

		context->RSSetState(RSstate);
	}
};


// Depth-Stencil state object
class DXDepthStencilState : public DXPipelineStage {

	D3D11_DEPTH_STENCIL_DESC								dsDesc;
	ID3D11DepthStencilState									*dsState = nullptr;

public:

	// Initialise default depth-stencil state object
	DXDepthStencilState(ID3D11Device *device) {

		ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

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

		HRESULT hr = device->CreateDepthStencilState(&dsDesc, &dsState);

		if (!SUCCEEDED(hr))
			throw std::exception("Cannot create Depth-Stencil state interface");
	}

	// Initialise depth-stencil state object based on the given descriptor
	DXDepthStencilState(ID3D11Device *device, const D3D11_DEPTH_STENCIL_DESC& desc) {

		ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		memcpy_s(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC), &desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

		HRESULT hr = device->CreateDepthStencilState(&dsDesc, &dsState);

		if (!SUCCEEDED(hr))
			throw std::exception("Cannot create Depth-Stencil state interface");
	}

	~DXDepthStencilState() {

		if (dsState)
			dsState->Release();
	}

	void apply(ID3D11DeviceContext *context) {

		context->OMSetDepthStencilState(dsState, 0);
	}
};


// Blend state object
class DXBlendState : public DXPipelineStage {

	D3D11_BLEND_DESC										blendDesc;
	ID3D11BlendState										*blendState = nullptr;

	FLOAT													blendFactor[4];
	UINT													sampleMask = 0xFFFFFFFF; // Bitwise flags to determine which samples to process in an MSAA context

public:

	// Initialise default blend state object
	DXBlendState(ID3D11Device * device) {

		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

		// Setup default blend state descriptor (no blend)
		blendDesc.AlphaToCoverageEnable = FALSE; // Use pixel coverage info from rasteriser (default)
		blendDesc.IndependentBlendEnable = FALSE; // The following array of render target blend properties uses the blend properties from RenderTarget[0] for ALL render targets

		blendDesc.RenderTarget[0].BlendEnable = FALSE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&blendDesc, &blendState);

		if (!SUCCEEDED(hr))
			throw std::exception("Cannot create Blend state interface");

		blendFactor[0] = blendFactor[1] = blendFactor[2] = blendFactor[3] = 1.0f;
	}

	// Initialise blend state object based on the given descriptor
	DXBlendState(ID3D11Device *device, const D3D11_BLEND_DESC& desc) {

		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
		memcpy_s(&blendDesc, sizeof(D3D11_BLEND_DESC), &desc, sizeof(D3D11_BLEND_DESC));

		HRESULT hr = device->CreateBlendState(&blendDesc, &blendState);

		if (!SUCCEEDED(hr))
			throw std::exception("Cannot create Blend state interface");

		blendFactor[0] = blendFactor[1] = blendFactor[2] = blendFactor[3] = 1.0f;
	}

	// Initialise blend state object based on the given descriptor, blend factor and sample mask
	DXBlendState(ID3D11Device *device, const D3D11_BLEND_DESC& desc, const DirectX::XMFLOAT4& _blendFactor, const UINT _sampleMask) {

		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
		memcpy_s(&blendDesc, sizeof(D3D11_BLEND_DESC), &desc, sizeof(D3D11_BLEND_DESC));

		HRESULT hr = device->CreateBlendState(&blendDesc, &blendState);

		if (!SUCCEEDED(hr))
			throw std::exception("Cannot create Blend state interface");

		blendFactor[0] = _blendFactor.x;
		blendFactor[1] = _blendFactor.y;
		blendFactor[2] = _blendFactor.z;
		blendFactor[3] = _blendFactor.w;

		sampleMask = _sampleMask;
	}

	~DXBlendState() {

		if (blendState)
			blendState->Release();
	}

	void apply(ID3D11DeviceContext *context) {

		context->OMSetBlendState(blendState, blendFactor, sampleMask);
	}
};



//
// Pipeline stages / objects that are dependent upon the host window extent
//

// Base interface for pipeline stages that are dependent upon window extent
class DXWindowExtentStage : public DXPipelineStage {

public:

	// Initialise class
	static bool Initialise();

	// Shut down class
	static void Shutdown();

	// Broadcast window extent changes to all descendents of DXWindowExtentStage
	static void Resize(UINT width, UINT height);

private:

	virtual void _resize(const UINT width, const UINT height) = 0;
};


// Declare rs_viewport_lambda - instances of this are called during window resize events to re-configure the viewports accordingly.  This allows a host (generic) viewport stage instance to specialise viewport configuration without needing a sub-class for each different viewport setup the application needs.

typedef std::function<void(UINT, UINT, UINT*, D3D11_VIEWPORT**)> dx_viewport_lambda;


// Viewport configuration object
class DXViewport : public DXWindowExtentStage {

	UINT							numViewports = 0;
	D3D11_VIEWPORT					*viewportArray = nullptr;
	
	// Resize viewport lambda (see above)
	dx_viewport_lambda				vpResize;

	// Note: Add scissor rect configuration later

public:

	// Initialise a single viewport based on the given dimensions
	DXViewport(const UINT width, const UINT height);

	// Initialise a single viewport based on the given D3D11_VIEWPORT descriptor
	DXViewport(const D3D11_VIEWPORT& vpDesc);

	// Setup array of viewports - a resize lambda is expected since it is left to the application to decide how an array of viewports should be arranged
	DXViewport(const UINT _numViewports, const D3D11_VIEWPORT *_vpArray, dx_viewport_lambda& _resizeLambda);

	~DXViewport();

	void apply(ID3D11DeviceContext *context) {

		context->RSSetViewports(numViewports, viewportArray);
	}

	// Handle window resize event
	void _resize(const UINT width, const UINT height) {

		if (numViewports>0 && viewportArray!=nullptr)
			vpResize(width, height, &numViewports, &viewportArray);
	}
};


// Render target object
class DXRenderTarget : public DXWindowExtentStage {

	// Maintain references to render target and depth stencil view hosts
	// These are optional.  If no render target hosts are present then renderTargetHosts.size() = 0
	std::vector<DXViewHost<ID3D11RenderTargetView>*>		renderTargetHosts;
	DXViewHost<ID3D11DepthStencilView>						*depthStencilHost = nullptr;

	// Derived interface lists from render target host list (above)
	UINT													numRenderTargets = 0;
	ID3D11RenderTargetView									**renderTargetViews = nullptr;

	// Note: Add UAVs later

	//
	// Private interface
	//

	// At render time we need a contiguous array of render target views (RTV) when we call OMSetRenderTargetViews on the D3D context.  The actual views are stored indirectly via DXViewHost instances in the renderTargetHosts vector and these might have been changed due to a window resize event, so rebuild the contiguous RTV list from the current view interfaces held by the render target hosts used by this OM stage instance.
	void rebuildRTVListFromHosts();

public:

	DXRenderTarget(ID3D11Device *device, const std::vector<DXViewHost<ID3D11RenderTargetView>*> &rtvList, DXViewHost<ID3D11DepthStencilView> *dsViewHost);
	
	~DXRenderTarget();

	void apply(ID3D11DeviceContext *context) {

		context->OMSetRenderTargets(numRenderTargets, renderTargetViews, (depthStencilHost) ? depthStencilHost->getResourceView() : nullptr);
	}

	// Handle window resize event
	void _resize(const UINT width, const UINT height) {

		rebuildRTVListFromHosts();
	}
};
