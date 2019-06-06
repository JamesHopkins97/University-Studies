#pragma once
class Model;
class Quad;
class Effect;
class BlurUtility
{

	int										blurWidth = 512;
	int										blurHeight = 512;
	D3D11_VIEWPORT							offScreenViewport;
	ID3D11ShaderResourceView				*intermedHSRV = nullptr;
	ID3D11RenderTargetView					*intermedHRTV = nullptr;
	ID3D11ShaderResourceView				*intermedVSRV = nullptr;
	ID3D11RenderTargetView					*intermedVRTV = nullptr;
	ID3D11ShaderResourceView				*intermedSRV = nullptr;
	ID3D11RenderTargetView					*intermedRTV = nullptr;
	ID3D11Texture2D							*depthStencilBufferOrb = nullptr;
	ID3D11DepthStencilView					*depthStencilViewOrb = nullptr;
	ID3D11DeviceContext						*context = nullptr;
	ID3D11Device							*device = nullptr;
	Quad									*screenQuad = nullptr;

	// from glow tutorial
	//ID3D11DepthStencilState					*orbDSstate = nullptr;
	ID3D11BlendState						*alphaOnBlendState = nullptr;
	ID3D11VertexShader						*screenQuadVS = nullptr;
	ID3D11PixelShader						*emissivePS = nullptr;
	ID3D11PixelShader						*horizontalBlurPS = nullptr;
	ID3D11PixelShader						*verticalBlurPS = nullptr;
	ID3D11PixelShader						*textureCopyPS = nullptr;
	ID3D11PixelShader						*depthCopyPS = nullptr;
	ID3D11VertexShader						*perPixelLightingVS = nullptr;

	Effect									*defaultEffect = nullptr;

public:
	BlurUtility(ID3D11Device *deviceIn, ID3D11DeviceContext *contextIn);
	HRESULT setupBlurRenderTargets();
	void blurModel(Model*orb, ID3D11ShaderResourceView	*depthSRV);
	~BlurUtility();
};

