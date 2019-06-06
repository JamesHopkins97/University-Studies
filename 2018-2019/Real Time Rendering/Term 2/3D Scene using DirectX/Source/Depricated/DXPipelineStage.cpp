
#include "stdafx.h"
#include <DXPipelineStage.h>

using namespace std;


// Global (singleton) list of pipeline stages that depend on window extent - these are descendants of DXWindowExtentStage
static vector<DXWindowExtentStage*> *resizeStageObjects = nullptr;



//
// Class and private methods that handle pipeline stages
//


// Initialise pipeline stage list
bool DXWindowExtentStage::Initialise() {

	// Instantiate pipeline vector (NOT THREAD SAFE)
	if (resizeStageObjects == nullptr)
		resizeStageObjects = new vector<DXWindowExtentStage*>();

	return (resizeStageObjects != nullptr) ? true : false;
}

// Shut down class
void DXWindowExtentStage::Shutdown() {

	if (resizeStageObjects)
		delete resizeStageObjects;
}

void DXWindowExtentStage::Resize(UINT width, UINT height) {

	for (DXWindowExtentStage *E : *resizeStageObjects) {

		if (E)
			E->_resize(width, height);
	}
}

void StoreStageObject(DXWindowExtentStage *_s) {

	if (resizeStageObjects && _s)
		resizeStageObjects->push_back(_s);
}

void RemoveStageObject(DXWindowExtentStage *_s) {

	if (resizeStageObjects && _s) {

		auto i = std::find(resizeStageObjects->begin(), resizeStageObjects->end(), _s);

		if (i != resizeStageObjects->end())
			resizeStageObjects->erase(i);
	}
}


//
// DXViewport constructors / destructors
//


// Initialise a single viewport based on the given dimensions
DXViewport::DXViewport(const UINT width, const UINT height) {

	viewportArray = (D3D11_VIEWPORT *)malloc(sizeof(D3D11_VIEWPORT));

	if (!viewportArray)
		throw exception("Cannot create viewport array");

	D3D11_VIEWPORT *viewport = viewportArray;

	viewport->TopLeftX = 0;
	viewport->TopLeftY = 0;
	viewport->Width = static_cast<FLOAT>(width);
	viewport->Height = static_cast<FLOAT>(height);
	viewport->MinDepth = 0.0f;
	viewport->MaxDepth = 1.0f;

	numViewports = 1;

	vpResize = [&](UINT width, UINT height, UINT *numViewports, D3D11_VIEWPORT **viewportArray)->void {

		// For this lambda we're only accessing a single viewport so numViewports is unused
		D3D11_VIEWPORT *vp = *viewportArray;

		vp->Width = static_cast<FLOAT>(width);
		vp->Height = static_cast<FLOAT>(height);
	};

	// Store weak reference to stage object in class list
	StoreStageObject(this);
}


// Initialise a single viewport based on the given D3D11_VIEWPORT descriptor
DXViewport::DXViewport(const D3D11_VIEWPORT& vpDesc) {

	viewportArray = (D3D11_VIEWPORT *)malloc(sizeof(D3D11_VIEWPORT));

	if (!viewportArray)
		throw exception("Cannot create viewport array");

	D3D11_VIEWPORT *viewport = viewportArray;

	viewport->TopLeftX = vpDesc.TopLeftX;
	viewport->TopLeftY = vpDesc.TopLeftY;
	viewport->Width = vpDesc.Width;
	viewport->Height = vpDesc.Height;
	viewport->MinDepth = vpDesc.MinDepth;
	viewport->MaxDepth = vpDesc.MaxDepth;

	numViewports = 1;

	vpResize = [&](UINT width, UINT height, UINT *numViewports, D3D11_VIEWPORT **viewportArray)->void {

		// For this lambda we're only accessing a single viewport so numViewports is unused
		D3D11_VIEWPORT *vp = *viewportArray;

		vp->Width = static_cast<FLOAT>(width);
		vp->Height = static_cast<FLOAT>(height);
	};

	// Store weak reference to stage object in class list
	StoreStageObject(this);
}


// Initialise array of viewports - a resize lambda is expected since it is left to the application to decide how an array of viewports should be arranged
DXViewport::DXViewport(const UINT _numViewports, const D3D11_VIEWPORT *_vpArray, dx_viewport_lambda& _resizeLambda) {

	if (_numViewports == 0 || !_vpArray)
		return;

	viewportArray = (D3D11_VIEWPORT *)malloc(_numViewports * sizeof(D3D11_VIEWPORT));

	if (!viewportArray)
		throw exception("Cannot create viewport array");

	memcpy_s(viewportArray, _numViewports * sizeof(D3D11_VIEWPORT), _vpArray, _numViewports * sizeof(D3D11_VIEWPORT));
	numViewports = _numViewports;

	vpResize = _resizeLambda;

	// Store a weak reference to the viewport stage in the global DXWindowExtentStage class list
	StoreStageObject(this);
}


DXViewport::~DXViewport() {

	if (viewportArray)
		free(viewportArray);

	// Reomve 'this' from the global DXWindowExtentStage class list
	RemoveStageObject(this);
}




//
// DXRenderTarget constructors / destructors
//


//
// Private interface
//

// At render time we need a contiguous array of render target views (RTV) when we call OMSetRenderTargetViews on the D3D context.  The actual views are stored indirectly via DXViewHost instances in the renderTargetHosts vector and these might have been changed due to a window resize event, so rebuild the contiguous RTV list from the current view interfaces held by the render target hosts used by this OM stage instance.
void DXRenderTarget::rebuildRTVListFromHosts() {

	if (renderTargetViews) {

		free(renderTargetViews);
		renderTargetViews = nullptr;
	}

	if (numRenderTargets > 0) {

		renderTargetViews = (ID3D11RenderTargetView**)malloc(numRenderTargets * sizeof(ID3D11RenderTargetView*));

		if (renderTargetViews) {

			for (UINT i = 0; i < numRenderTargets; ++i) {

				// Note: Only store weak reference to interfaces - rtvTargetHosts indirectly stores reference to actual interface via host object.
				renderTargetViews[i] = renderTargetHosts[i]->getResourceView();
			}

		}
		else {

			// Force to 0 since array cannot be created
			numRenderTargets = 0;
		}
	}
}


DXRenderTarget::DXRenderTarget(ID3D11Device *device, const std::vector<DXViewHost<ID3D11RenderTargetView>*> &rtvList, DXViewHost<ID3D11DepthStencilView> *dsViewHost) {

	for (auto i : rtvList) {

		renderTargetHosts.push_back(i);

		if (i)
			i->retain();
	}

	// Store single depth-stencil view host
	depthStencilHost = dsViewHost;

	if (depthStencilHost)
		depthStencilHost->retain();

	//
	// Derive actual DX interface lists for render-time
	//
	numRenderTargets = renderTargetHosts.size();
	rebuildRTVListFromHosts();

	// Store a weak reference to the render target stage in the global DXWindowExtentStage class list
	StoreStageObject(this);
}


DXRenderTarget::~DXRenderTarget() {

	for (auto i : renderTargetHosts) {

		if (i)
			i->release();
	}

	// Dispose of depth-stencil view host 
	if (depthStencilHost)
		depthStencilHost->release();

	// Release interface list (since weak reference - see above - don't need to release)
	if (renderTargetViews)
		free(renderTargetViews);

	// Reomve 'this' from the global DXWindowExtentStage class list
	RemoveStageObject(this);
}
