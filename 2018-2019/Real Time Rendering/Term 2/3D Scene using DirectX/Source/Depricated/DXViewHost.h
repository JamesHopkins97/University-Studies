
//
// DXViewHost.h
//

// DXViewHost stores a reference to a D3D11 resource view interface.  DXViewHost retains a reference to the contained interface.  This decouples the view from each pipeline object simplifying view resizing for example.

#pragma once

#include <d3d11_2.h>
#include <GUObject.h>


template <class T>
class DXViewHost : public GUObject {

	T		*resourceView;

public:

	DXViewHost() {

		resourceView = nullptr;
	}

	DXViewHost(T *_resourceView) {

		resourceView = _resourceView;

		if (resourceView)
			resourceView->AddRef();
	}

	~DXViewHost() {

		if (resourceView)
			resourceView->Release();
	}

	T* getResourceView() {
		
		return resourceView;
	}


	void releaseResourceView() {

		if (resourceView) {

			resourceView->Release();
			resourceView = nullptr;
		}
			
	}


	void setResourceView(T *_newResourceView) {

		if (_newResourceView)
			_newResourceView->AddRef();

		if (resourceView)
			resourceView->Release();

		resourceView = _newResourceView;
	}

};
