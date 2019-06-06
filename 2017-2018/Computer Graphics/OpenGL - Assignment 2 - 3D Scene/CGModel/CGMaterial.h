// Define core material library structure

#pragma once

#include "CGImport3.h"
#include <glew\glew.h>
#include <glew\wglew.h>
#include <CoreStructures\CoreStructures.h>

// define material constants
#define	CG_MATERIAL_NAMELENGTH			32
#define	CG_MAXMATERIALS					255

struct CGIMPORT3_API CGMaterial {
	char					materialName[CG_MATERIAL_NAMELENGTH];
	
	GLfloat					ambientRed, ambientGreen, ambientBlue, ambientAlpha;
	GLfloat					diffuseRed, diffuseGreen, diffuseBlue, diffuseAlpha;
	GLfloat					specularRed, specularGreen, specularBlue, specularAlpha;
	GLfloat					emissiveRed, emissiveGreen, emissiveBlue, emissiveAlpha;

	GLfloat					shininess; // [0.0, 128.0] - additional specular light property
	GLfloat					reflection; // [0, 1] - reflection coefficient
	GLfloat					refraction; // [0, 1] - refraction coefficient
	bool					doubleSided;


// CGMaterial API

public:

	bool materialHasName(const std::string& cname) const {
		return (strcmp(cname.c_str(), materialName)==0);
	}

};

