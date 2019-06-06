#include "stdafx.h"
#include "Texture.h"
#include <iostream>
#include <exception>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;
Texture::Texture(ID3D11Device *device, const std::wstring& filename)
{
	SRV = nullptr;
	ID3D11Resource *resource = static_cast<ID3D11Resource*>(texture);
	HRESULT hr;
	// Get filename extension
	wstring ext = filename.substr(filename.length() - 4);

	try
	{
		if (0 == ext.compare(L".bmp") || 0 == ext.compare(L".jpg") || 0 == ext.compare(L".png") || 0 == ext.compare(L".tif"))
			hr = CreateWICTextureFromFile(device, filename.c_str(), &resource, &SRV);
		else if (0 == ext.compare(L".dds"))
			hr = CreateDDSTextureFromFile(device, filename.c_str(), &resource, &SRV);
		else throw exception("Texture file format not supported");
	}
	catch (exception& e)
	{
		cout << "Texture was not loaded:\n";
		cout << e.what() << endl;
	}

	texture = static_cast<ID3D11Texture2D*>(resource);
}


Texture::~Texture()
{
}
