
#pragma once
#pragma warning( disable : 4275 )
#pragma warning( disable : 4251 )
#pragma warning( disable : 4231 )

#include "CGPolyMesh.h"
#include "CGMaterial.h"
#include <map>

CGEXP_TEMPLATE_API template class CGIMPORT3_API std::vector<CGPolyMesh*>;
CGEXP_TEMPLATE_API template class CGIMPORT3_API std::vector<CGMaterial>;



class CGIMPORT3_API CGModel : public CoreStructures::GUObject {

private:
	// STL containers used for model storage
	std::vector<CGPolyMesh*>	meshLibrary;
	std::vector<CGMaterial>		materialLibrary;

	// STL containers for texture model storage
	// CGModel does not assume any ownership of the texture objects - the host app / library is responsible for calling glDeleteTexture on any loaded texture objects
	std::map<CGPolyMesh*, uint32_t>		textureMaps; // associative array to map meshes to (diffuse) textures
	std::map<CGPolyMesh*, uint32_t>		normalMaps; // associative array to map meshes to normal map textures

public:
	CGModel(void);
	~CGModel(void);

	// Mesh-related methods
	void addMesh(CGPolyMesh *newMesh); // check syntax for const pointers
	int noofTextureCoords();
	
	int getMeshCount();
	CGPolyMesh *getMeshAtIndex(int i);

	void invertNormals();

	// Ensure a 1:1 correspondance between texture coordinates and vertex coordinates
	void mapToTextureTopology();

	// Material-related methods
	void addMaterial(const CGMaterial &newMaterial);
	int getIndexOfMaterialWithName(char *mtlName);

	void setTextureForMesh(CGPolyMesh *M, uint32_t textureID); // associate mesh *M to texture textureID.  It is assumed M is contained within the host CGModel
	void setTextureForModel(uint32_t textureID); // associate texture textureID to all meshes contained within the CGModel
	void setNormalMapForMesh(CGPolyMesh *M, uint32_t textureID); // associate mesh *M to normal map textureID.  It is assumed M is contained within the host CGModel
	void setNormalMapForModel(uint32_t textureID); // associate normal map textureID to all meshes contained within the CGModel

	void removeTextureFromMesh(CGPolyMesh *M);
	void removeTexturesFromModel();
	void removeNormalMapFromMesh(CGPolyMesh *M);
	void removeNormalMapsFromModel();


	// Rendering methods

	void render(); // render mesh with just vertex and normal data applied

	void renderTexturedModel(); // render textured model with vertex, normal and texture coordinate (not multi-textured) data.  If no texture is associated with a given mesh then render() is called instead.  This method assumes texture state has been setup.  This includes glEnable(GL_TEXTURE_2D) and glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, <mode>) for example	

	void renderWithMaterials(); // render mesh with per-face material applied

	void renderNormalMapped(uint32_t normalMapShader);


	// Normal map creation and rendering methods
	void createVertexTangentVectors2();
	void renderTangentSpaceBasis();
	

	// Transformation methods
	void translate(const CoreStructures::GUVector4 &T);
	void scale(const CoreStructures::GUVector4 &S);
	void rotate(const CoreStructures::GUQuaternion &q);

};
