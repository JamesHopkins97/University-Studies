#pragma once

#if 1

#include "CGImport3.h"
#include <CGModel\CGModel.h>
#include <CoreStructures\CoreStructures.h>
#include "CGIntersect2D.h"


// Model a simple 2-manifold terrain (ie. a surface) in 3D space.  CGTerrain is a subclass of CGModel.
// Constraints: To simplify processing and terrain queries, only the first mesh object stored will be considered the terrain.
// This is only an issue for multi-component file structures such as 3DS and gsf.  obj and md2 files are always loaded as
// a single mesh anyway.  This behavior can be changed if necessary.


struct CGIMPORT3_API CGEdge {

	int			v1, v2, f1, f2;
};

struct CGIMPORT3_API CGFaceEdge {

	int			e1, e2, e3;
};


// structs for terrain query methods

struct CGIMPORT3_API CGLocalSurfaceLocation {

public:

	int									faceIndex;
	CoreStructures::GUVector4			location;
	CoreStructures::GUMatrix4			R; // local orientation information at point on surface

public:

	void init() {

		faceIndex = -1; // -1 means no intersection
		location.define(0.0f, 0.0f, 0.0f, 1.0f);
		R = CoreStructures::GUMatrix4::I();
	}
};



class CGIMPORT3_API CGTerrain : public CGModel {

private:

	// terrain connectivity model.  This uses a simple, but redundant edge connectivity model to link faces and
	// vertices together.  For those who are interested, see the half-edge, winged-edge and later quad-edge structures
	// for better topology models.

	int				noofEdges;
	CGEdge			*E;
	CGFaceEdge		*Fe; // associate faces to edges - order(n), where n is the number of faces in the FIRST mesh (see above notes)

public:

	// Constructors / Destructors
	CGTerrain(void);
	~CGTerrain(void);

	// Accessor methods
	int edgeCount(void);

	// Terrain query methods
	float getHeight(CoreStructures::GUVector4 *v);
	CGLocalSurfaceLocation getPointOnSurface(CoreStructures::GUVector4 *v);
	CGLocalSurfaceLocation getPointForFace(int fi);
	void moveAcrossTerrain(CGLocalSurfaceLocation *L, CoreStructures::GUVector4 *D);
	void getNormalForFace(int fi, CoreStructures::GUVector4 *N);

	// render methods
	void renderConnectivity(void);

	// topology creation methods
	bool createConnectivityInfo(void);

private:

	void init(void);
	void dispose(void);
	
	void projectInitialFace(int fi, CGBaseMeshDefStruct *mDef, CoreStructures::GUVector2 *uv1, CoreStructures::GUVector2 *uv2, CoreStructures::GUVector2 *uv3, CoreStructures::GUVector4 *initPosition, CoreStructures::GUVector4 *displacementVector, CoreStructures::GUVector2 *posR2, CoreStructures::GUVector2 *dispR2);
	void projectConnectedFace(int fi, int p1index, const CoreStructures::GUVector2& uv1, int p2index, const CoreStructures::GUVector2& uv2, CGBaseMeshDefStruct *mDef, int *p3index, CoreStructures::GUVector2 *uv3);
	int getNextFace(int fi, CGBaseMeshDefStruct *mDef, int v1index, int v2index);

	CGLocalSurfaceLocation getIntersectionForPoint(CoreStructures::GUVector4 *p);
	double getIntersectionForRay(CoreStructures::GUVector4 *v1, CoreStructures::GUVector4 *v2, CGLocalSurfaceLocation *L);

	void createTangentSpaceForFace(int fi, CoreStructures::GUMatrix4 *R);
	void createTangentSpaceForFace(int fi, CoreStructures::GUMatrix4 *R, CGBaseMeshDefStruct *mDef);
};

#endif
