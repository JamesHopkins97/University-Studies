//
// renders our scene made up of shapes using VBO's, multiple textures, translation and two shaders
//

#include "stdafx.h"
#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include "texture_loader.h"
#include "shader_setup.h"
#include <math.h>
#include <ctime>
#include <chrono>

using namespace std;
using namespace CoreStructures;


// Globals
GLuint locT; // location of "T" uniform variable in myShaderProgram
GLuint locT2;

// Declares Textures
GLuint background = 0;
GLuint grass = 1;
GLuint castle = 2;
GLuint window = 3;
GLuint door = 4;
GLuint knight = 5;
GLuint spear = 6;
GLuint flag = 7;
float rotation = -3;

static int countNum = 0;


GLuint myShaderProgram; // Shader program object for applying textures to our shapes
GLuint myShaderProgramNoTexture; // Second shader progam object for non textured shapes such as our rainbow star

// Vertex Buffer Object IDs for the background, the grass, the castle.
GLuint backgroundPosVBO, backgroundTexCoordsVBO, backgroundIndicesVBO;
GLuint grassPosVBO, grassTexCoordsVBO, grassIndicesVBO;
GLuint windowPosVBO, windowTexCoordsVBO, windowIndicesVBO;
GLuint doorPosVBO, doorTexCoordsVBO, doorIndicesVBO;
GLuint knightPosVBO, knightTexCoordsVBO, knightIndicesVBO;
GLuint spearPosVBO, spearTexCoordsVBO, spearIndicesVBO;

//Packed vertex arrays for the background
static GLfloat backgroundVertices[] = {

	-1.0, -1.f,
	-1.0f, 1.f,
	1.0f, -1.f,
	1.0f, 1.f
};
static float backgroundTextureCoords[] = {

	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f

};
static GLubyte backgroundVertexIndices[] = { 0, 1, 2, 3 };

//Packed vertex arrays for the Grass
static GLfloat grassVertices[] = {
	-1.0, -1.f,
	-1.0f, 1.f,
	1.0f, -1.f,
	1.0f, 1.f
};
static float grassTextureCoords[] = {
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f
};
static GLubyte grassVertexIndices[] = { 0, 1, 2, 3 }; 

//Packed vertex arrays for the castle windows
static GLfloat windowVertices[] = {
	-0.1, -0.3,
	-0.1, 0.3,
	0.1, -0.3,
	0.1, 0.3
};
static float windowTextureCoords[] = {
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f
};
static GLubyte windowVertexIndices[] = { 0, 1, 2, 3 };

//Packed vertex arrays for the casle door
static GLfloat doorVertices[] = {
	-0.2, -0.3,
	-0.2, 0.4,
	0.2, -0.3,
	0.2, 0.4
};
static float doorTextureCoords[] = {
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f
};
static GLubyte doorVertexIndices[] = { 0, 1, 2, 3 };

//Packed vertex arrays for the knight
static GLfloat knightVertices[] = {
		-0.1, -0.3,
	-0.1, 0.3,
	0.1, -0.3,
	0.1, 0.3
};
static float knightTextureCoords[] = {
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f
};
static GLubyte knightVertexIndices[] = { 0, 1, 2, 3 };

//Packed vertex arrays for the spear
static GLfloat spearVertices[] = {
	1, -0.05,
	1, 0.05,
	0.1, -0.05,
	0.1, 0.05
};
static float spearTextureCoords[] = {
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f
};
static GLubyte spearVertexIndices[] = { 0, 1, 2, 3 };

// Function Prototypes
void init(int argc, char* argv[]);
void setupBackgroundTextureVBO(void);
void setupGrassVBO(void);
void setupWindowTextureVBO(void);
void setupDoorTextureVBO(void);
void setupKnightTextureVBO(void);
void setupSpearTextureVBO(void);
void report_version(void);
void display(void);
void drawTexturedQuadVBOBackground(void);
void drawTexturedQuadVBOGrass(void);
void drawWindowVBOTexture(void);
void drawDoorVBOTexture(void);
void drawKnightVBOTexture(void);
void drawSpearVBOTexture(void);
void drawFlag(void);
float knightMovement();
void drawSun(void);
void drawCastle(void);
void update(void);
float AIMovement(void);
float setYPos(void);
float randomNumberGenerator();

int _tmain(int argc, char* argv[]) {

	init(argc, argv);

	glutMainLoop();

	// Shut down COM
	shutdownCOM();
	
	return 0;
}

void init(int argc, char* argv[]) {

	// Initialise COM so we can use Windows Imaging Component
	initCOM();

	// Initialise FreeGLUT
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(1280, 720);
	glutInitWindowPosition(256, 144);
	glutCreateWindow("Coursework - Castle Scene");

	// Register callback functions
	glutDisplayFunc(display);
	glutIdleFunc(update);
	// Initialise GLEW library
	GLenum err = glewInit();

	// Ensure GLEW was initialised successfully before proceeding
	if (err == GLEW_OK) {

		cout << "GLEW initialised okay\n";

	} else {

		cout << "GLEW could not be initialised\n";
		throw;
	}
	
	// Example query OpenGL state (get version number)
	report_version();


	// Report maximum number of vertex attributes
	GLint numAttributeSlots;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributeSlots);
	cout << "GL_MAX_VERTEX_ATTRIBS = " << numAttributeSlots << endl;

	// Initialise OpenGL...

	// Setup colour to clear the window
	glClearColor(0, 0, 0, 0.0f);

	glLineWidth(1.0f);

	// Load demo texture
	background = fiLoadTexture("sky.jpg");
	grass = fiLoadTexture("grass.png");
	castle = fiLoadTexture("castle.jpeg");
	window = fiLoadTexture("window.jpg");
	door = fiLoadTexture("door.jpg");
	knight = fiLoadTexture("knight.png");
	spear = fiLoadTexture("spear.jpg");
	flag = fiLoadTexture("flag.png");
	// Shader setup 
	myShaderProgram = setupShaders(string("Shaders\\basic_vertex_shader.txt"), string("Shaders\\basic_fragment_shader.txt"));
	myShaderProgramNoTexture = setupShaders(string("Shaders\\notexture_vertex_shader.txt"), string("Shaders\\notexture_fragment_shader.txt"));

	// Get uniform location of "T" variable in shader program (we'll use this in the play function to give the uniform variable "T" a value)
	locT = glGetUniformLocation(myShaderProgram, "T");
	locT2 = glGetUniformLocation(myShaderProgramNoTexture, "T2");

	// Setup star object using Vertex Buffer Objects (VBOs)
	setupBackgroundTextureVBO();
	setupGrassVBO();
	setupWindowTextureVBO();
	setupDoorTextureVBO();
	setupKnightTextureVBO();
	setupSpearTextureVBO();
}

void report_version(void) {

	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";
}


// our ground
void setupBackgroundTextureVBO(void) {
	// setup VBO for the quad object position data
	glGenBuffers(1, &backgroundPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, backgroundPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
}

void setupGrassVBO(void) {
	// setup VBO for the quad object position data
	glGenBuffers(1, &grassPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);

	// setup VBO for the quad object texture coord data
	glGenBuffers(1, &grassTexCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassTexCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassTextureCoords), grassTextureCoords, GL_STATIC_DRAW);

	// setup quad vertex index array
	glGenBuffers(1, &grassIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(grassVertexIndices), grassVertexIndices, GL_STATIC_DRAW);
}

void setupWindowTextureVBO(void)
{
	//VBO for window position
	glGenBuffers(1, &windowPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, windowPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertices), windowVertices, GL_STATIC_DRAW);
	
	//VBO for window texture coordinates
	glGenBuffers(1, &windowTexCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, windowTexCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(windowTextureCoords), windowTextureCoords, GL_STATIC_DRAW);

	//VBO for window vertex array
	glGenBuffers(1, &windowIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, windowIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(windowVertexIndices), windowVertexIndices, GL_STATIC_DRAW);
}

void setupDoorTextureVBO(void) {
	//VBO for door position
	glGenBuffers(1, &doorPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, doorPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(doorVertices), doorVertices, GL_STATIC_DRAW);

	//VBO for door texture coords
	glGenBuffers(1, &doorTexCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, doorTexCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(doorTextureCoords), doorTextureCoords, GL_STATIC_DRAW);

	//VBO for door vertex array
	glGenBuffers(1, &doorIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, doorIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(doorVertexIndices), doorVertexIndices, GL_STATIC_DRAW);
}

void setupKnightTextureVBO(void) {
	//VBO for knight position
	glGenBuffers(1, &knightPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, knightPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(knightVertices), knightVertices, GL_STATIC_DRAW);

	//VBO for knight texture coords
	glGenBuffers(1, &knightTexCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, knightTexCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(knightTextureCoords), knightTextureCoords, GL_STATIC_DRAW);

	//VBO for knight vertex array
	glGenBuffers(1, &knightIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, knightIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(knightVertexIndices), knightVertexIndices, GL_STATIC_DRAW);
}

void setupSpearTextureVBO(void) {
	//VBO for spear position
	glGenBuffers(1, &spearPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, spearPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(spearVertices), spearVertices, GL_STATIC_DRAW);

	//VBO for spear texture coords
	glGenBuffers(1, &spearTexCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, spearTexCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(spearTextureCoords), spearTextureCoords, GL_STATIC_DRAW);

	//VBO for spear vertex array
	glGenBuffers(1, &spearIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spearIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(spearVertexIndices), spearVertexIndices, GL_STATIC_DRAW);
}

// Example rendering functions
void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// render our scene in sequence
	// draw our objects
	drawTexturedQuadVBOBackground();

	drawTexturedQuadVBOGrass();

	drawSun();

	drawCastle();

	drawFlag();
	
	drawWindowVBOTexture();

	drawDoorVBOTexture();

	drawKnightVBOTexture();

	drawSpearVBOTexture();

	glutSwapBuffers();
}

// draw our ground function
void drawTexturedQuadVBOBackground(void) {
	
	glUseProgram(myShaderProgram);

	// Move our ground shape to the bottom half of the screen
	GUMatrix4 T = GUMatrix4::translationMatrix(0.0f, 0.f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, background);
	glUniform1i(glGetUniformLocation(myShaderProgram, "Background"), 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, backgroundPosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Bind the index buffer

	// Draw the object - same function call as used for vertex arrays but the last parameter is interpreted as an offset into the currently bound index buffer (set to 0 so we start drawing from the beginning of the buffer).
	glDrawElements(GL_QUAD_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	// use to force disable our shaderprogram
 	// glUseProgram(0);

}
void drawTexturedQuadVBOGrass(void) {
	glUseProgram(myShaderProgram);

	// Move our ground shape to the bottom half of the screen
	GUMatrix4 T = GUMatrix4::translationMatrix(0.0f, -1.6f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);


	// Bind each vertex buffer and enable
	// The data is still stored in the GPU but we need to set it up (which also includes validation of the VBOs behind-the-scenes)

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass);
	glUniform1i(glGetUniformLocation(myShaderProgram, "Grass"), 0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glBindBuffer(GL_ARRAY_BUFFER, grassPosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, grassColourVBO);
	//glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	//glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, grassTexCoordsVBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);


	// Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassIndicesVBO);

	// Draw the object - same function call as used for vertex arrays but the last parameter is interpreted as an offset into the currently bound index buffer (set to 0 so we start drawing from the beginning of the buffer).
	glDrawElements(GL_QUAD_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	// use to force disable our shaderprogram
	// glUseProgram(0);

}
void drawWindowVBOTexture(void) {
	glUseProgram(myShaderProgram);
	int count = 0;
	float MatrixX = -0.45;
	while (count < 2) // draws two windows opposite each other
	{
		// Move our ground shape to the bottom half of the screen
		GUMatrix4 T = GUMatrix4::translationMatrix(MatrixX, -0.1f, 0.0f);
		glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);

		// Bind texture to windows
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, window);
		glUniform1i(glGetUniformLocation(myShaderProgram, "window"), 0);
		glEnable(GL_TEXTURE_2D);

		glBindBuffer(GL_ARRAY_BUFFER, windowPosVBO);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Bind the index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, windowIndicesVBO);

		// Draw the object - same function call as used for vertex arrays but the last parameter is interpreted as an offset into the currently bound index buffer (set to 0 so we start drawing from the beginning of the buffer).
		glDrawElements(GL_QUAD_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

		glDisable(GL_TEXTURE_2D);
		count++;
		MatrixX++;
	}
}
void drawDoorVBOTexture(void) {
	glUseProgram(myShaderProgram);

	//Changes the Y coordinate to fit in the castle
	GUMatrix4 T = GUMatrix4::translationMatrix(0.f, -0.3f, 0.f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);

	//Binds the door texture to the doorVBO vertices
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, door);
	glUniform1i(glGetUniformLocation(myShaderProgram, "door"), 0);
	glEnable(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, doorPosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Bind the index buffer for door
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, doorIndicesVBO);
	//Draws using GL_QUAD_STRIP
	glDrawElements(GL_QUAD_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glDisable(GL_TEXTURE_2D);

}
void drawKnightVBOTexture(void) {
	glUseProgram(myShaderProgram);

	//Updates to check for changes in the x and y coordinate
	GUMatrix4 T = GUMatrix4::translationMatrix(-1.f, -0.3, 0.f) * GUMatrix4::translationMatrix(knightMovement(), 0.0, 0.f) * GUMatrix4::rotationMatrix(0, rotation, 0);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);
	
	//Binds the knight texture to the KnightVBO vertices
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, knight);
	glUniform1i(glGetUniformLocation(myShaderProgram, "texture"), 0);
	glEnable(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, knightPosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Bind the index buffer for knight
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, knightIndicesVBO);
	//Draws the knight using GL_QUAD_STRIP
	glDrawElements(GL_QUAD_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);
	glDisable(GL_TEXTURE_2D);

}
void drawSpearVBOTexture(void) {
	glUseProgram(myShaderProgram);
	
	//Updates to check for changes in the x and y coordinate
	GUMatrix4 T = GUMatrix4::translationMatrix(-1.f, -0.3, 0.f) * GUMatrix4::translationMatrix(AIMovement(), setYPos(), 0.f) * GUMatrix4::rotationMatrix(0, -180, 0);
	//GUMatrix4 rot = GUMatrix4::rotationMatrix(0, -3, 0);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);
	//glUniformMatrix4fv(locT2, 1, GL_FALSE, (GLfloat*)&pos);

	//Binds the spear texture to the spearVBO vertices
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, spear);
	glUniform1i(glGetUniformLocation(myShaderProgram, "spear"), 0);
	glEnable(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, spearPosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Bind the index buffer for door
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spearIndicesVBO);
	//Draws the spear using GL_QUAD_STRIP
	glDrawElements(GL_QUAD_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);
	glDisable(GL_TEXTURE_2D);
}

void drawSun(void) {

	glUseProgram(0);
	glEnable(GL_BLEND);
	glBegin(GL_TRIANGLE_FAN);

	float theta;
	float x;
	float y;

	//Draws the sun, using a for loop to adjust the X and Y positions accordingly
	for (float i = 0; i < 10; i += 0.1)
	{
		glColor3ub(255, 150, 0);

		x = ((1 * cos(i)) / 4);
		y = ((1 * sin(i)) / 4);
		//set the coord, dividing the X coordinate to adjust to the screen resolution
		glVertex2f((x / 1.7) - 0.8, y + 0.7);
	}

	glEnd();
	glDisable(GL_BLEND);
}
void drawCastle(void) {
	//Draws the castle using GL_TRIANGLE_FAN rendering mode with the castle texture binded to the shape
	glBindTexture(GL_TEXTURE_2D, castle);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(-0.8, -0.6f);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(-0.8, 0.4);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(0.8, 0.4);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(0.8, -0.6f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(-0.8, 0.4f);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(-0.8, 0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(-0.7, 0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(-0.7, 0.4);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(-0.6, 0.4f);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(-0.6, 0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(-0.5, 0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(-0.5, 0.4f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(-0.4, 0.4);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(-0.4, 0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(-0.3, 0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(-0.3, 0.4);
	glEnd();
	
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(-0.2, 0.4);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(-0.2, 0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(-0.1, 0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(-0.1, 0.4);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(0.1, 0.4);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(0.1, 0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(0.2, 0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(0.2, 0.4);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(0.3, 0.4);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(0.3, 0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(0.4, 0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(0.4, 0.4);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(0.5, 0.4);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(0.5, 0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(0.6, 0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(0.6, 0.4);
	glEnd();
	
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(0.7, 0.4);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(0.7, 0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(0.8, 0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(0.8, 0.4f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
void drawFlag(void) {
	//Draws the white pole above the castle using GL_QUADS rendering mode
	glBegin(GL_QUADS);
	glColor3ub(255, 255, 255);
	glVertex2f(-0.55, 0.85f);
	glVertex2f(-0.55, 0.5);
	glVertex2f(-0.56, 0.5f);
	glVertex2f(-0.56, 0.85);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3ub(0, 255, 0);
	glVertex2f(-0.548, 0.55);
	glColor3ub(255, 0, 0);
	glVertex2f(-0.548, 0.85);
	glColor3ub(0, 0, 255);
	glVertex2f(-0.3, 0.7);
	glEnd();

}

void update(void)
{
	//Redraws the screen
	glutPostRedisplay();
}

float knightMovement()
{
	//creates speed variable to default
	static float speed = 0.001;

	//if user press Left arrow key, move X Position to the left, and change rotation so knight is facing left
	if (GetAsyncKeyState(VK_LEFT))
	{
		speed -= 0.01;
		rotation = 0;
	}

	//if user press Right arrow key, move X Position to the right, and change rotation so knight is facing right
	if (GetAsyncKeyState(VK_RIGHT))
	{
		speed += 0.01;
		rotation = -3;
	}
	//cout << speed << endl;

	return speed;
}

float AIMovement()
{
	//sets spear X Position
	static float speed = 0.001;

	//Increments speed by .05
	speed += 0.05;
	
	if (speed >= 3) //if speed is more than 3 it resets it to -1, this shows the spear off changing position off screen.
	{
		speed = -1;
	}
	//cout << speed << endl;

	return speed; //return X Coordinate
}


float setYPos()
{
	//create YPos variable 
	float YPos = 0.01;
	//Set YPos to the returning value of RandomNumberGenerator()
	YPos = randomNumberGenerator();
	//YPosDiv being YPos divided by 10 (E.g. if RandomNumberGenerator() returns 3, it calculates to 0.3
	float YPosDiv = YPos / 10;
	//print the Y Coordinate for the spear
	cout << YPosDiv << endl;
	//return YPosDiv
	return YPosDiv;
}

float randomNumberGenerator() //Randomly generates number from 1-10
{
	float rng;
	srand(static_cast<int>(time(NULL)));
	rand();
	rng = rand() % 10; //sets num to a number randomised between 1 and 10
	rng / 10;
	return rng;
}