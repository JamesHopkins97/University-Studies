
//
// DXPipeline.h
//

// Encapsulate a DirectX rendering pipeline configuration using DXPipelineStage objects.  These in turn encapsulate shader interfaces, Rasterisation (RS) and Output Merger (OM) state objects.  Input Assesmbler (IA) configuration is left for per-object, or per-batch setup at render-time.


#pragma once

#include <d3d11_2.h>
#include <GUObject.h>
#include <vector>


class DXPipelineStage;
struct DXStreamOutConfig;
class DXBlob;


class DXPipeline : public GUObject {

	std::vector<DXPipelineStage*>			pipelineStages;
	
public:

	// Initialise the DXPipeline class and associated class resources - this must be called before any DXPipeline or DXPipelineStage methods are called
	static bool InitialisePipelineModel();

	// Close down the DXPipeline class and associated class resources - called prior to the application terminating
	static void ShutdownPipelineModel();

	// Broadcast to window extent dependent pipeline stages the new host window (HWND) size
	static void ResizeTargets(UINT width, UINT height);

	// Apply a single pipeline stage to the given context
	static void Apply(ID3D11DeviceContext *context, DXPipelineStage* S);

	// Apply a given std::vector of pipeline stages to the given context
	static void Apply(ID3D11DeviceContext *context, std::vector<DXPipelineStage*>& stageList);

	// Apply a given DXPipeline instance
	static void Apply(ID3D11DeviceContext *context, DXPipeline *P);

	// Clear the rendering pipeline by removing all interfaces
	static void Clear(ID3D11DeviceContext *context);


	// Constructor - take a null-terminated list of DXPipelineStage objects and add to the pipeline object.  The pipeline retains ownership of the contained pipeline stage objects.
	DXPipeline(DXPipelineStage *e1, ...);

	~DXPipeline();
};
