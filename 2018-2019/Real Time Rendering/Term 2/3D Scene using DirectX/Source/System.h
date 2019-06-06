
//
// System.h
//

// Encapsulates D3D device independent, device dependent and window size dependent resources.  For now this supports a single swap chain so there exists a 1:1 correspondance between the System instance and the associated window.
#pragma once
#include <d3d11_2.h>
#include <windows.h>


class System {

	// Use DXGI 1.1
	IDXGIFactory1							*dxgiFactory = nullptr;
	IDXGISwapChain							*swapChain = nullptr;

	IDXGIAdapter							*defaultAdapter = nullptr;
	ID3D11Device							*device = nullptr;
	ID3D11DeviceContext						*context = nullptr;
	D3D_FEATURE_LEVEL						supportedFeatureLevel;
	
	ID3D11RenderTargetView					*renderTargetView = nullptr;
	ID3D11DepthStencilView					*depthStencilView = nullptr;
	ID3D11Texture2D							*depthStencilBuffer = nullptr;
	ID3D11ShaderResourceView				*depthStencilSRV = nullptr;

	// Private interface

	// Private constructor
	System(HWND hwnd);

public:

	// Public interface

	// System factory method
	static System* CreateDirectXSystem(HWND hwnd);

	// Destructor
	~System();

	// Setup DirectX interfaces that will be constant - they do not depend on any device
	HRESULT setupDeviceIndependentResources();
	// Setup DirectX interfaces that are dependent upon a specific device
	HRESULT setupDeviceDependentResources(HWND hwnd);
	// Setup window-specific resources including swap chain buffers, texture buffers and resource views that are dependant upon the host window size
	HRESULT setupWindowDependentResources(HWND hwnd);

	// Update methods

	// Resize swap chain buffers according to the given window client area
	HRESULT resizeSwapChainBuffers(HWND hwnd);
	// Present back buffer to the screen
	HRESULT presentBackBuffer();

	// Accessor methods
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
	ID3D11RenderTargetView* getBackBufferRTV();
	ID3D11DepthStencilView* getDepthStencil();
	ID3D11Texture2D* getDepthStencilBuffer();
	ID3D11ShaderResourceView* getDepthStencilSRV() { return depthStencilSRV; }

};
