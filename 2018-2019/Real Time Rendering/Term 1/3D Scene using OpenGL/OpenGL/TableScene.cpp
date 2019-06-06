#include "TableScene.h"
#include "TextureLoader.h"
#include "ShaderLoader.h"
#include <iostream>



using namespace std;



TableScene::TableScene(int sWidth, int sHeight, int sSample) {

	screenHeight = sHeight * sSample;
	screenWidth = sWidth * sSample;

	// Camera settings
	//							  width, heigh, near plane, far plane
	Camera_settings camera_settings{ screenWidth, screenHeight, 0.1, 100.0 };


	tableModel = new Model("Resources\\Models\\Table\\table2.obj");

	chairModel = new Model("Resources\\Models\\Chair\\chair2.obj");

	// Instanciate the camera object with basic data
	earthCamera = new Camera(camera_settings, glm::vec3(0.0, 0.0, 5.0));

	//
	// Setup textures for rendering the Earth model
	//


	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Phong_shader.vs"),
		string("Resources\\Shaders\\Phong_shader.fs"),
		&phongShader);


	// Setup uniform locations for shader

	dayTextureUniform = glGetUniformLocation(phongShader, "texture0");

	modelMatrixLocation = glGetUniformLocation(phongShader, "modelMatrix");

	invTransposeMatrixLocation = glGetUniformLocation(phongShader, "invTransposeModelMatrix");

	viewProjectionMatrixLocation = glGetUniformLocation(phongShader, "viewProjectionMatrix");

	lightDirectionLocation = glGetUniformLocation(phongShader, "dir1.lightDirection");

	lightDiffuseLocation = glGetUniformLocation(phongShader, "dir1.lightDiffuseColour");

	lightSpecularLocation = glGetUniformLocation(phongShader, "dir1.lightSpecularColour");

	lightSpecExpLocation = glGetUniformLocation(phongShader, "dir1.lightSpecularExponent");

	cameraPosLocation = glGetUniformLocation(phongShader, "cameraPos");


	// Set constant uniform data (uniforms that will not change while the application is running)
	// Note: Remember we need to bind the shader before we can set uniform variables!
	glUseProgram(phongShader);

	//glUniform1i(dayTextureUniform, 0);

	glUseProgram(0);


	//
	// Setup FBO (which Earth rendering pass will draw into)
	//

	glGenFramebuffers(1, &demoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, demoFBO);

	//
	// Setup textures that will be drawn into through the FBO
	//

	// Setup colour buffer texture.
	// Note:  The texture is stored as linear RGB values (GL_RGBA8).  
	//There is no need to pass a pointer to image data - 
	//we're going to fill in the image when we render the Earth scene at render time!
	glGenTextures(1, &fboColourTexture);
	glBindTexture(GL_TEXTURE_2D, fboColourTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	// Setup depth texture

	glGenTextures(1, &fboDepthTexture);
	glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	//
	// Attach textures to the FBO
	//

	// Attach the colour texture object to the framebuffer object's colour attachment point #0
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		fboColourTexture,
		0);

	// Attach the depth texture object to the framebuffer object's depth attachment point
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		fboDepthTexture,
		0);


	//
	// Before proceeding make sure FBO can be used for rendering
	//

	GLenum demoFBOStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (demoFBOStatus != GL_FRAMEBUFFER_COMPLETE) {

		fboOkay = false;
		cout << "Could not successfully create framebuffer object to render texture!" << endl;

	}
	else {

		fboOkay = true;
		cout << "FBO successfully created" << endl;
	}

	// Unbind FBO for now! (Plug main framebuffer back in as rendering destination)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


TableScene::~TableScene() {

}


// Accessor methods

Camera* TableScene::getTableSceneCamera() {

	return earthCamera;
}


GLuint TableScene::getTableSceneTexture() {

	return fboColourTexture;
}


float TableScene::getSunTheta() {

	return 0.0f;
}


void TableScene::updateSunTheta(float thetaDelta) {

	//sunTheta += thetaDelta;
}


// Scene update
void TableScene::update(const float timeDelta) {


}

//Renders the models
void TableScene::modelRender(Model* m, glm::mat4 T, glm::mat4 transform) {
	if (m) {

		// Modelling transform
		glm::mat4 modelTransform = transform;

		// Calculate inverse transpose of the modelling transform for correct transformation of normal vectors
		glm::mat4 inverseTranspose = glm::transpose(glm::inverse(modelTransform));;

		glUseProgram(phongShader);

		// Get the location of the camera in world coords and set the corresponding uniform in the shader
		glm::vec3 cameraPos = earthCamera->getCameraPosition();
		glUniform3fv(cameraPosLocation, 1, (GLfloat*)&cameraPos);

		// Set the model, view and projection matrix uniforms (from the camera data obtained above)
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
		glUniformMatrix4fv(invTransposeMatrixLocation, 1, GL_FALSE, glm::value_ptr(inverseTranspose));
		glUniformMatrix4fv(viewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(T));



		// Activate and Bind the textures to texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dayTexture);

		//Render the model
		m->draw(phongShader);

		// Restore default OpenGL shaders (Fixed function operations)
		glUseProgram(0);
	}
}
// Rendering methods

void TableScene::render() {

	if (!fboOkay)
		return; // Don't render anything if the FBO was not created successfully

				// Bind framebuffer object so all rendering redirected to attached images (i.e. our texture)
	glBindFramebuffer(GL_FRAMEBUFFER, demoFBO);
	// All rendering from this point goes to the bound textures (setup at initialisation time) and NOT the actual screen!!!!!

	// Clear the screen (i.e. the texture)
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport to specified texture size (see above)
	glViewport(0, 0, 800, 800);

	// Get view-projection transform as a CGMatrix4
	glm::mat4 T = earthCamera->getProjectionMatrix() * earthCamera->getViewMatrix();

	glUseProgram(phongShader);
	// Set the light direction uniform vector in world coordinates based on the Sun's position
	glUniform4f(lightDirectionLocation, 2.0f, 2.0f, 0.0, 0.0f);

	glUniform4f(lightDiffuseLocation, 1.0f, 1.0f, 1.0f, 1.0f); // white diffuse light
	glUniform4f(lightSpecularLocation, 0.4f, 0.4f, 0.4f, 1.0f); // white specular light
	glUniform1f(lightSpecExpLocation, 8.0f); // specular exponent / falloff
	glUseProgram(0);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 modelScale = glm::scale(model, glm::vec3(0.078, 0.058, 0.078));
	glm::mat4 modelRotation3 = glm::rotate(model, 180.0f*(3.14f / 180), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 modelTrans = glm::translate(model, glm::vec3(1.0f, -1.3f, 0.0f));
	model = modelTrans * modelRotation3 * modelScale;

	glm::mat4 model2 = glm::mat4(1.0f);
	glm::mat4 modelScale2 = glm::scale(model2, glm::vec3(0.078, 0.058, 0.078));
	glm::mat4 modelRotation = glm::rotate(model2, 180.0f*(3.14f / 180), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 modelTrans2 = glm::translate(model2, glm::vec3(-1.0f, -1.3f, 0.0f));
	model2 = modelTrans2 * modelRotation *  modelScale2;

	glm::mat4 model3 = glm::mat4(1.0f);
	glm::mat4 modelScale3 = glm::scale(model3, glm::vec3(0.078, 0.058, 0.078));
	glm::mat4 modelTrans3 = glm::translate(model3, glm::vec3(1.0f, -1.3f, -2.0f));
	model3 = modelTrans3  * modelScale3;

	glm::mat4 model4 = glm::mat4(1.0f);
	glm::mat4 modelScale4 = glm::scale(model4, glm::vec3(0.078, 0.058, 0.078));
	glm::mat4 modelTrans4 = glm::translate(model4, glm::vec3(-1.0f, -1.3f, -2.0f));
	model4 = modelTrans4   * modelScale4;

	glm::mat4 table = glm::mat4(1.0f);
	glm::mat4 tableScale = glm::scale(table, glm::vec3(0.0525f, 0.0425f, 0.0525f));
	//glm::mat4 tableRotation = glm::rotate(table, 180.0f*(3.14f / 180), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 tableTrans = glm::translate(table, glm::vec3(0.0f, -1.0f, -0.75f));
	table = tableTrans * tableScale;

	//model Rendering
	modelRender(tableModel, T, table);
	modelRender(chairModel, T, model);
	modelRender(chairModel, T, model2);
	modelRender(chairModel, T, model3);
	modelRender(chairModel, T, model4);



	// Set OpenGL to render to the MAIN framebuffer (ie. the screen itself!!)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
