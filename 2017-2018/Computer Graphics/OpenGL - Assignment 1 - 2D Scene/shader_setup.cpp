
#include "stdafx.h"
#include "shader_setup.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

using namespace std;


// private function declarations

static GLSL_ERROR createShaderFromFile(GLenum shaderType, const string& shaderFilePath, GLuint *shaderObject, const string **shaderSource);
static const string *shaderSourceStringFromFile(const string& filePath);
static void printSourceListing(const string& sourceString, bool showLineNumbers=true);
static void reportProgramInfoLog(GLuint program);
static void reportShaderInfoLog(GLuint shader);


// main shader loader function

GLuint setupShaders(const string& vsPath, const string& fsPath, GLSL_ERROR *error_result) {

	GLuint					vertexShader=0, fragmentShader=0, glslProgram = 0;
	const string			*vertexShaderSource = NULL, *fragmentShaderSource = NULL;
	
	// create vertex shader object
	GLSL_ERROR err = createShaderFromFile(GL_VERTEX_SHADER, vsPath, &vertexShader, &vertexShaderSource);
	
	if (err != GLSL_OK) {
		
		switch(err) {
				
			case GLSL_SHADER_SOURCE_NOT_FOUND:
				
				printf("Vertex shader source not found.  Ensure the GUShaderSource object for the vertex shader has been created successfully.\n");

				if (error_result)
					*error_result = GLSL_VERTEX_SHADER_SOURCE_NOT_FOUND;
				
				return 0;
				

			case GLSL_SHADER_OBJECT_CREATION_ERROR:
				
				printf("OpenGL could not create the vertex shader program object.  Try using fewer resources before creating the program object.\n");

				if (error_result)
						*error_result = GLSL_VERTEX_SHADER_OBJECT_CREATION_ERROR;

				return 0;
				

			case GLSL_SHADER_COMPILE_ERROR:
				
				printf("The vertex shader could not be compiled successfully...\n");
				printf("Vertex shader source code...\n\n");
				
				printSourceListing(*vertexShaderSource);
				
				// report compilation error log
				
				printf("\n<vertex shader compiler errors--------------------->\n\n");
				reportShaderInfoLog(vertexShader);
				printf("<-----------------end vertex shader compiler errors>\n\n\n");
				
				// dispose of existing shader objects

				glDeleteShader(vertexShader);

				if (vertexShaderSource)
					delete vertexShaderSource;
				
				if (error_result)
					*error_result = GLSL_VERTEX_SHADER_COMPILE_ERROR;

				return 0;
				

			default:
				
				printf("The vertex shader object could not be created successfully.\n");

				// dispose of existing shader objects

				glDeleteShader(vertexShader);

				if (vertexShaderSource)
					delete vertexShaderSource;

				if (error_result)
					*error_result = err;

				return 0;
		}
	}
	
	
	// create fragment shader object
	err = createShaderFromFile(GL_FRAGMENT_SHADER, fsPath, &fragmentShader, &fragmentShaderSource);
	
	if (err != GLSL_OK) {
		
		switch(err) {
				
			case GLSL_SHADER_SOURCE_NOT_FOUND:
				
				printf("Fragment shader source not found.  Ensure the GUShaderSource object for the fragment shader has been created successfully.");

				// dispose of existing shader objects

				glDeleteShader(vertexShader);

				if (vertexShaderSource)
					delete vertexShaderSource;

				if (error_result)
					*error_result = GLSL_FRAGMENT_SHADER_SOURCE_NOT_FOUND;

				return 0;
				

			case GLSL_SHADER_OBJECT_CREATION_ERROR:
				
				printf("OpenGL could not create the fragment shader program object.  Try using fewer resources before creating the program object.\n");

				// dispose of existing shader objects

				glDeleteShader(vertexShader);

				if (vertexShaderSource)
					delete vertexShaderSource;

				if (error_result)
					*error_result = GLSL_FRAGMENT_SHADER_OBJECT_CREATION_ERROR;
				
				return 0;
				

			case GLSL_SHADER_COMPILE_ERROR:
				
				printf("The fragment shader could not be compiled successfully...\n");
				printf("Fragment shader source code...\n\n");
				
				printSourceListing(*fragmentShaderSource);
				
				// report compilation error log
				
				printf("\n<fragment shader compiler errors--------------------->\n\n");
				reportShaderInfoLog(fragmentShader);
				printf("<-----------------end fragment shader compiler errors>\n\n");
				
				// dispose of existing shader objects
				glDeleteShader(vertexShader);
				glDeleteShader(fragmentShader);

				if (vertexShaderSource)
					delete vertexShaderSource;

				if (fragmentShaderSource)
					delete fragmentShaderSource;

				if (error_result)
					*error_result = GLSL_FRAGMENT_SHADER_COMPILE_ERROR;

				return 0;
				

			default:
				
				printf("The fragment shader object could not be created successfully.\n");

				// dispose of existing shader objects
				glDeleteShader(vertexShader);
				glDeleteShader(fragmentShader);

				if (vertexShaderSource)
					delete vertexShaderSource;

				if (fragmentShaderSource)
					delete fragmentShaderSource;

				if (error_result)
					*error_result = err;

				return 0;
		}
	}
	
	// source code objects not longer needed
	if (vertexShaderSource)
		delete vertexShaderSource;

	if (fragmentShaderSource)
		delete fragmentShaderSource;

	//
	// Once vertex and fragment shader objects have been validated, setup the main
	// shader program object
	//
	glslProgram = glCreateProgram();
	
	if (!glslProgram) {
		
		printf("The shader program object could not be created.\n");
		
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		
		if (error_result)
			*error_result = GLSL_PROGRAM_OBJECT_CREATION_ERROR;

		return 0;
	}
	

	// Attach shader objects
	glAttachShader(glslProgram, vertexShader);
	glAttachShader(glslProgram, fragmentShader);
	

	// link the shader program
	glLinkProgram(glslProgram);
	

	// validate link status
	GLint linkStatus;

	glGetProgramiv(glslProgram, GL_LINK_STATUS, &linkStatus);
	
	if (linkStatus==0) {
		
		// failed to link - report linker error log and dispose of local resources

		printf("The shader program object could not be linked successfully...\n");
		
		// report linker error log
		
		printf("\n<GLSL shader program object linker errors--------------------->\n\n");
		reportProgramInfoLog(glslProgram);
		printf("<-----------------end shader program object linker errors>\n\n");
		
		// delete program and detach shaders
		glDeleteProgram(glslProgram);
		
		// delete shaders
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		
		if (error_result)
			*error_result = GLSL_PROGRAM_OBJECT_LINK_ERROR;

		return 0;
	}
	
	// cleanup - delete individual shader objects since they're now included as part of the overall shader program object
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	if (error_result)
		*error_result = GLSL_OK;

	return glslProgram;
}



//
// private function implementation
//

GLSL_ERROR createShaderFromFile(GLenum shaderType, const string& shaderFilePath, GLuint *shaderObject, const string **shaderSource) {

	const string *sourceString = shaderSourceStringFromFile(shaderFilePath);

	if (!sourceString)
		return GLSL_SHADER_SOURCE_NOT_FOUND;

	const char *src = sourceString->c_str();

	if (!src) {

		delete sourceString;
		return GLSL_SHADER_OBJECT_CREATION_ERROR;
	}

	GLuint shader = glCreateShader(shaderType);

	if (shader==0) {

		delete sourceString;
		return GLSL_SHADER_OBJECT_CREATION_ERROR;
	}

	glShaderSource(shader, 1, static_cast<const GLchar**>(&src), 0);
	glCompileShader(shader);
	
	*shaderObject = shader;
	*shaderSource = sourceString;

	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	
	return (compileStatus==0) ? GLSL_SHADER_COMPILE_ERROR : GLSL_OK;
}


const string *shaderSourceStringFromFile(const string& filePath) {

	string *sourceString = NULL;

	const char *file_str = filePath.c_str();

	struct stat fileStatus;
	int file_error = stat(file_str, &fileStatus);

	if (file_error==0) {

		_off_t fileSize = fileStatus.st_size;

		char *src = (char *)calloc(fileSize+1, 1); // add null-terminator character at end of string

		if (src) {

			ifstream shaderFile(file_str);

			if (shaderFile.is_open()) {

				shaderFile.read(src, fileSize);
				sourceString = new string(src);

				shaderFile.close();
			}

			// dispose of local resources
			free(src);
		}
	}

	// return pointer to new source string
	return sourceString;
}


void printSourceListing(const string& sourceString, bool showLineNumbers) {

	const char *srcPtr = sourceString.c_str();
	const char *srcEnd = srcPtr + sourceString.length();
	
	size_t lineIndex = 0;

	while (srcPtr < srcEnd) {

		if (showLineNumbers) {

			cout.fill(' ');
			cout.width(4);
			cout << dec << ++lineIndex << " > ";
		}

		size_t substrLength = strcspn(srcPtr, "\n");
		
		cout << string(srcPtr, 0, substrLength) << endl;

		srcPtr += substrLength + 1;
	}
}


void reportProgramInfoLog(GLuint program)
{	
	GLsizei			noofBytes = 0;
	
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &noofBytes);
	
	GLchar *str = (GLchar*)calloc(noofBytes+1, 1);
	
	if (str) {
		
		glGetProgramInfoLog(program, noofBytes, 0, str);
		printf("%s\n", str);
		
		free(str);
	}
}


void reportShaderInfoLog(GLuint shader)
{	
	GLsizei			noofBytes = 0;
	
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &noofBytes);
	
	GLchar *str = (GLchar*)calloc(noofBytes+1, 1);
	
	if (str) {
		
		glGetShaderInfoLog(shader, noofBytes, 0, str);
		printf("%s\n", str);
		
		free(str);
	}
}
