#ifndef Table_SCENE_H
#define Table_SCENE_H

#include "Sphere.h"
#include "Camera.h"
#include "Includes.h"


class Model;

class TableScene {

private:

	Model *tableModel;
	Model *chairModel;

	// Move around the earth with a seperate camera to the main scene camera
	Camera						*earthCamera;

	// Textures for multi-texturing the earth model
	GLuint							dayTexture;

	// Shader for multi-texturing the earth
	GLuint							phongShader;
	GLuint							SSShader;

	// Unifom locations for earthShader

	// Texture uniforms
	GLuint							dayTextureUniform;
	GLuint							nightTextureUniform;
	GLuint							maskTextureUniform;

	// Camera uniforms
	GLint							modelMatrixLocation;
	GLint							invTransposeMatrixLocation;
	GLint							viewProjectionMatrixLocation;

	// Directional light uniforms
	GLint							lightDirectionLocation;
	GLint							lightDiffuseLocation;
	GLint							lightSpecularLocation;
	GLint							lightSpecExpLocation;
	GLint							cameraPosLocation;


	// Screen variables
	int screenHeight;
	int screenWidth;


	//
	// Framebuffer Object (FBO) variables
	//

	// Actual FBO
	GLuint							demoFBO;

	// Colour texture to render into
	GLuint							fboColourTexture;

	// Depth texture to render into
	GLuint							fboDepthTexture;

	// Flag to indicate that the FBO is valid
	bool							fboOkay;

public:

	TableScene(int, int, int);
	~TableScene();

	// Accessor methods
	Camera* getTableSceneCamera();
	GLuint getTableSceneTexture();
	float getSunTheta();
	void updateSunTheta(float thetaDelta);
	void modelRender(Model* m, glm::mat4 T, glm::mat4 transform);
	// Scene update
	void update(const float timeDelta);

	// Rendering methods
	void render();
};

#endif