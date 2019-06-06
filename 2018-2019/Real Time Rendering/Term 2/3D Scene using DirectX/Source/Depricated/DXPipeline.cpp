
#include "stdafx.h"
#include <cstdarg>
#include <exception>
#include <DXPipeline.h>
#include <DXPipelineStage.h>

using namespace std;


// Initialise the DXPipeline class and associated class resources - this must be called before any DXPipeline or DXPipelineStage methods are called
bool DXPipeline::InitialisePipelineModel() {

	// Setup DXWindowExtentStage class list
	return DXWindowExtentStage::Initialise();
}


// Close down the DXPipeline class and associated class resources - called prior to the application terminating
void DXPipeline::ShutdownPipelineModel() {

	// Destroy DXWindowExtentStage class list
	DXWindowExtentStage::Shutdown();
}


// Broadcast to window extent dependent pipeline stages the new host window (HWND) size
void DXPipeline::ResizeTargets(UINT width, UINT height) {

	DXWindowExtentStage::Resize(width, height);
}


// Apply a single pipeline stage to the given context
void DXPipeline::Apply(ID3D11DeviceContext *context, DXPipelineStage* S) {

	if (S)
		S->apply(context);
}


// Apply a given std::vector of pipeline stages to the given context
void DXPipeline::Apply(ID3D11DeviceContext *context, std::vector<DXPipelineStage*>& stageList) {

	for (auto i : stageList) {
		
		i->apply(context);
	}
}


// Apply a given DXPipeline instance
void DXPipeline::Apply(ID3D11DeviceContext *context, DXPipeline *P) {

	DXPipeline::Apply(context, P->pipelineStages);
}


// Clear the rendering pipeline by removing all interfaces
void DXPipeline::Clear(ID3D11DeviceContext *context) {

	context->VSSetShader(nullptr, nullptr, 0);
	context->HSSetShader(nullptr, nullptr, 0);
	context->DSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
	context->CSSetShader(nullptr, nullptr, 0);

	context->RSSetState(nullptr);
	context->RSSetViewports(0, nullptr);

	context->OMSetDepthStencilState(nullptr, 0);
	context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	context->OMSetRenderTargets(0, nullptr, nullptr);
}


// Constructor - take a null-terminated list of DXPipelineStage objects and add to the pipeline object.  The pipeline retains ownership of the contained pipeline stage objects.
DXPipeline::DXPipeline(DXPipelineStage *s1, ...) {

	if (!s1)
		throw exception("Cannot create DXPipeline instance - at least one valid DXPipelineStage object must be provided");

	DXPipelineStage *s = s1;
	s->retain();
	pipelineStages.push_back(s);

	va_list stageList;
	va_start(stageList, s1);

	while (s != nullptr) {

		s = va_arg(stageList, DXPipelineStage*);

		if (s) {

			s->retain();
			pipelineStages.push_back(s);
		}
	}

	va_end(stageList);
}


DXPipeline::~DXPipeline() {

	for (auto i : pipelineStages) {

		if (i)
			i->release();
	}
}
