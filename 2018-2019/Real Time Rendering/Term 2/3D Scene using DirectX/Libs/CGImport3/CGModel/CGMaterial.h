// Define core material library structure

#pragma once

#include <CGImport3\CGImport3.h>
#include <CoreStructures\CoreStructures.h>

// define material constants
#define	CG_MATERIAL_NAMELENGTH			32
#define	CG_MAXMATERIALS					255

struct CGIMPORT3_API CGMaterial {

	char					materialName[CG_MATERIAL_NAMELENGTH];
	
	float					ambientRed, ambientGreen, ambientBlue, ambientAlpha;
	float					diffuseRed, diffuseGreen, diffuseBlue, diffuseAlpha;
	float					specularRed, specularGreen, specularBlue, specularAlpha;
	float					emissiveRed, emissiveGreen, emissiveBlue, emissiveAlpha;

	float					shininess; // [0.0, 128.0] - additional specular light property
	float					reflection; // [0, 1] - reflection coefficient
	float					refraction; // [0, 1] - refraction coefficient
	bool					doubleSided;


// CGMaterial API

public:

	bool materialHasName(const std::string& cname) const {
		return (strcmp(cname.c_str(), materialName)==0);
	}

};

