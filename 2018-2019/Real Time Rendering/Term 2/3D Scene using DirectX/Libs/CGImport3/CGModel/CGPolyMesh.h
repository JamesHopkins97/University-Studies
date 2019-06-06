//
//  CGPolyMesh.h
//
//  Implement basic mesh model - This provides basic geometry, material and texture coordinate storage
//

#pragma once
#pragma warning( disable : 4275 )

#include <CGImport3\CGImport3.h>
#include <CoreStructures\CoreStructures.h>
#include <vector>
#include <cstdint>

//
// Geometry definitions
//

// CGFaceVertex associates faces with consitituent vertices and vertex normals.
// <v1, v2, v3> are ordered counter-clockwise.
struct CGIMPORT3_API CGFaceVertex {
    int					v1, v2, v3;
};


// typedef to represent face-vertex normal associations.
typedef CGFaceVertex CGFaceNormal;



//
// Material definitions
//

// CGMaterialNode stores the material for a given face (0: no material; >=1: index of material)
struct CGIMPORT3_API CGMaterialNode {

	uint8_t					materialID;
};



//
// Texture coordinate definitions
//

// Note: CGTextureCoord defined in CoreStructures

// CGFaceTexture stores the texture coordinate indices for a given face.
// This is stored separately from vertex indices since a 1:m vertex-texture coord associativity can exist.
struct CGIMPORT3_API CGFaceTexture {
	int				t1, t2, t3;
};


// CGBaseMeshDefStruct is used to pass CGPolyMesh parameters to constructors.
// This avoids long formal and actual parameter lists.
struct CGIMPORT3_API CGBaseMeshDefStruct {
	int					N;
	int					n;
	CoreStructures::GUVector4			*V;
	CGFaceVertex		*Fv;
	CoreStructures::GUVector4			*Fn;
	CoreStructures::GUVector4			*Vn;
	CGMaterialNode		*Ma;    
	int					VtSize;
	CoreStructures::CGTextureCoord		*Vt;
	CGFaceTexture		*Fvt;
	CoreStructures::GUVector4			*T;

public:
	void init(void)
	{
		N = 0;
		n = 0;
		V = NULL;
		Fv = NULL;
		Fn = NULL;
		Vn = NULL;
		Ma = NULL;
		VtSize = 0;
		Vt = NULL;
		Fvt = NULL;
		T = NULL;
	}


	void dispose(void)
	{
		if (V)
			free(V);
		if (Fv)
			free(Fv);
		if (Fn)
			free(Fn);
		if (Vn)
			free(Vn);
		if (Ma)
			free(Ma);
		if (Vt)
			free(Vt);
		if (Fvt)
			free(Fvt);
		if (T)
			free(T);

		init();
	}


};

struct CGMaterial;

class CGIMPORT3_API CGPolyMesh : public CoreStructures::GUObject {

protected:
	int					N;				// noofVertices
	int					n;				// noofFaces;

	CoreStructures::GUVector4			*V;				// vertices
	CGFaceVertex		*Fv;			// face vertex indices

	CoreStructures::GUVector4			*Fn;			// face normals
	CoreStructures::GUVector4			*Vn;			// vertex normals (always 1:1 correspondence with vertices so noof normals = N)
	
	CGMaterialNode		*Ma;			// per-face material indices

	int					VtSize;			// number of points in texture coordinate list
	CoreStructures::CGTextureCoord		*Vt;			// vertex texture coords - 1:m relationship between vertex and texture coords assumed
	CGFaceTexture		*Fvt;			// per-face texture coordinate indices

	// Normal mapping data
	CoreStructures::GUVector4			*T;				// tangent vector array - order VtSize

//
//  Define CGPolyMesh interface
//

public:
	// class initialisation
	CGPolyMesh();
	CGPolyMesh(CGBaseMeshDefStruct *R); // designated initialiser (adopt objective-c practice)
	CGPolyMesh(CGPolyMesh *om); //copy constructor

	~CGPolyMesh();

	// Accessor methods (only provide read-access to buffers)
	int vertexCount(); // N
	int faceCount(); // n
	int noofTextureCoords() const; // VtSize

	CoreStructures::GUVector4 *vertexArray(); // V
	CGFaceVertex *vertexIndexArray(); // Fv
	CoreStructures::GUVector4 *faceNormalArray(); // Fn
	CoreStructures::GUVector4 *vertexNormalArray(); // Vn
	CGMaterialNode *materialArray(); // Ma
	int numberOfUniqueTextureCoordinates(); // VtSize
	CoreStructures::CGTextureCoord *textureCoordArray(); // Vt
	CGFaceTexture *faceTextureCoordArray(); // Fvt
	CoreStructures::GUVector4 *tangentArray(); // T

	// General functionality

	// Topology Query and Processing methods
	virtual bool isaValidMesh(bool checkNormals); // checkNormals is depricated
	int getDisjointVertex(CGFaceVertex *fvp, int vi, int vj);
	int getDisjointVertex(int fi, int vi, int vj);
	int maximumValence();
	
	// Ensure a 1:1 correspondance between texture coordinates and vertex coordinates
	void mapToTextureTopology();

	// Face and vertex normal calculation
	virtual void calculateFaceNormals(); // Create face normal array Fn.  This assumes the vertices on each face are ordered counter-clockwise
	virtual void calculateVertexNormals();
	virtual void invertNormals();
	virtual void createVertexNormalList(CoreStructures::GUVector4 *fn);
	
	// Transformation methods
	void translateAllVertices(const CoreStructures::GUVector4 &T);
	void scaleAllVertices(const CoreStructures::GUVector4 &S, const CoreStructures::GUVector4 &cPos);
	void rotateAllVertices(const CoreStructures::GUQuaternion &q, const CoreStructures::GUVector4 &cPos);

	// Derived quantities
	float surfaceArea();
	float faceArea(int fi);
	bool mapFaceToR2(int fi, CoreStructures::CGTextureCoord *t1, CoreStructures::CGTextureCoord *t2, CoreStructures::CGTextureCoord *t3);

	void assimilateMeshDef(CGBaseMeshDefStruct *R);
	void createMeshDef(CGBaseMeshDefStruct *R);

	// Depricated
	//void report(FILE *fp) const;

	// rendering methods
	void render();
	void renderTexturedMesh();
	void renderWithMaterials(std::vector<CGMaterial> matList);
	void renderNormalMapped(uint32_t normalMapShader);

	// normal map creation methods
	void createVertexTangentVectors2();
	void renderTangentSpaceBasis();

	// private normal map creation methods
private:
	void processTangentVertex(int tj, int tjnext, int vj, int vjnext, bool *B);
	void renderVertexTangentBasis(int tj, int vj, bool *B);

protected:
	void initCGPolyMesh();
	void disposeCGPolyMesh();
	void copyMeshData(CGBaseMeshDefStruct *R, bool createNormals);
	void createDefaultMaterials();
};

// auxialiary (public) functions to support geometry creation and manipulation
CGIMPORT3_API void disposeCGBaseMeshDef(CGBaseMeshDefStruct *R);

