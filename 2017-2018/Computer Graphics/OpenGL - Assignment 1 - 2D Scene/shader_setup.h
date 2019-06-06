
//
// Load and compile OpenGL Shading Language (GLSL) shaders
//

#pragma once

#include "glew\glew.h"
#include <string>

// Declare GLSL setup return / error codes
typedef enum GLSL_ERROR_CODES {
	
	GLSL_OK = 0,
	
	GLSL_VERTEX_SHADER_REQUIRED_ERROR, // Returned if a geometry shader but no vertex shader is defined
	
	GLSL_SHADER_SOURCE_NOT_FOUND, // Shader source string not found
	GLSL_SHADER_OBJECT_CREATION_ERROR, // Shader object cannot be created by OpenGL
	GLSL_SHADER_COMPILE_ERROR, // Shader object could not be compiled

	GLSL_VERTEX_SHADER_SOURCE_NOT_FOUND,
	GLSL_GEOMETRY_SHADER_SOURCE_NOT_FOUND,
	GLSL_FRAGMENT_SHADER_SOURCE_NOT_FOUND,
	
	GLSL_VERTEX_SHADER_OBJECT_CREATION_ERROR,
	GLSL_GEOMETRY_SHADER_OBJECT_CREATION_ERROR,
	GLSL_FRAGMENT_SHADER_OBJECT_CREATION_ERROR,
	
	GLSL_VERTEX_SHADER_COMPILE_ERROR,
	GLSL_GEOMETRY_SHADER_COMPILE_ERROR,
	GLSL_FRAGMENT_SHADER_COMPILE_ERROR,
	
	GLSL_PROGRAM_OBJECT_CREATION_ERROR,
	GLSL_PROGRAM_OBJECT_LINK_ERROR
	
} GLSL_ERROR;


// Basic shader object creation function takes a path to a vertex shader file and fragment shader file and returns a bound and linked shader program object
GLuint setupShaders(const std::string& vsPath, const std::string& fsPath, GLSL_ERROR *error_result=NULL);
