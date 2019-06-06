
//
// Week 6 - Demo 01 - Retained mode rendering 01 (Packed Arrays)
//

#include "stdafx.h"
#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include "texture_loader.h"
#include "shader_setup.h"
#include "CGModel\CGModel.h"
#include "Importers\CGImporters.h"
#include "Importers\3dsImporter.h"

using namespace std;
using namespace CoreStructures;

#define FIXED_FUNCTION false

// Globals

// Mouse input (rotation) example
// Variable we'll use to animate (rotate) our star object
float theta = 0.0f;
GUClock cgClock;
// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int mouse_x, mouse_y;
bool mDown = false;

// uniform variables to access in shader
GLuint gDiffuse; // Shader light diffuse
GLuint gAmbient; // Shader light ambient
GLuint gAttenuation; // Shader light attenuation
GLuint gLPosition; // Shader light position

GLuint gEyePos; // Shader eye position

//Material Properties
GLuint gMatAmbient;
GLuint gMatDiffuse;
GLuint gMatSpecular;

// Shader Matrix setup
GLuint gModelMatrix;
GLuint gViewMatrix;
GLuint gProjectionMatrix;
GLuint gNormalMatrix;

// Shader program object
GLuint myShaderProgram;

// Textures
GLuint myTexture = 0;
GLuint myTexture2 = 0;
GLuint WallTexture = 0;
GLuint SingerTexture = 0;
GLuint MicStandTexture = 0;

// Chair Models
CGModel *ChairModel1 = NULL;
CGModel *ChairModel2 = NULL;
CGModel *ChairModel3 = NULL;

// Chair Model Instance Matrix
GUMatrix4 ChairModel1Matrix;
GUMatrix4 ChairModel2Matrix;
GUMatrix4 ChairModel3Matrix;

// Wall Models
CGModel *Wall = NULL;
CGModel *Wall2 = NULL;
CGModel *Wall3 = NULL;
CGModel *Wall4 = NULL;
// Wall Instance Matrix
GUMatrix4 wallMatrix;
GUMatrix4 wall2Matrix;
GUMatrix4 wall3Matrix;
GUMatrix4 wall4Matrix;

CGModel *MicStand = NULL;
GUMatrix4 micStandMatrix;

CGModel *Singer = NULL;
GUMatrix4 singerMatrix;

CGModel *myModel2 = NULL;
GUMatrix4 myModelMatrix2;

CGModel *myModel3 = NULL;
GUMatrix4 myModelMatrix3;

GUPivotCamera *mainCamera = NULL;

// Lights
GLfloat light_ambient[] = { 0.01, 0.01, 0.01, 1};  
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };  
GLfloat light_specular[] = { 1.0 }; // White highlight { 1.0, 1.0, 1.0, 1.0 }
GLfloat light_position[] = { 0}; // Point light (w=1.0)

// Materials
GLfloat mat_amb_diff[] = { 1 }; // Texture map will provide ambient and diffuse.
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; // White highlight
GLfloat mat_shininess[] = { 150.0 };				 // Shiny surface
GLfloat mat_emission[] = { 0.0, 0.0, 0.0, 0.0 }; // Not emissive

// Function Prototypes
void init(int argc, char* argv[]);
void report_version(void);
void update(void);
void display(void);
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void keyDown(unsigned char key, int x, int y);
void mouseWheelUpdate(int wheel_number, int direction, int x, int y);
void ImportModels();

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

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(32, 32);
	glutCreateWindow("Set the Stage!");
	//glutFullScreen();

	// Register callback functions
	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutMouseFunc(mouseButtonDown);
	glutMotionFunc(mouseMove);
	glutMouseWheelFunc(mouseWheelUpdate);
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

	// Initialise OpenGL...
	// Setup colour to clear the window
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// setup main 3D camera
	mainCamera = new GUPivotCamera(0.0f, 0.0f, 5.0f, 55.0f, 1.0f, 0.1f);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glLineWidth(4.0f);

	// Shader setup - more on this next week!!!
	myShaderProgram = setupShaders(string("Shaders\\diffuse_vertex_shader.txt"), string("Shaders\\diffuse_fragment_shader.txt"));

	// get unifom locations in shader
	gLPosition = glGetUniformLocation(myShaderProgram, "lposition");
	gAmbient = glGetUniformLocation(myShaderProgram, "lambient");
	gDiffuse = glGetUniformLocation(myShaderProgram, "ldiffuse");
	gAttenuation = glGetUniformLocation(myShaderProgram, "lattenuation");
	gEyePos = glGetUniformLocation(myShaderProgram, "eyePos");

	// get material unifom locations in shader
	gMatAmbient = glGetUniformLocation(myShaderProgram, "matAmbient");
	gMatDiffuse = glGetUniformLocation(myShaderProgram, "matDiffuse");
	gMatSpecular = glGetUniformLocation(myShaderProgram, "matSpecular");



	// get matrix unifom locations in shader
	gModelMatrix = glGetUniformLocation(myShaderProgram, "g_ModelMatrix");
	gViewMatrix = glGetUniformLocation(myShaderProgram, "g_ViewMatrix");
	gProjectionMatrix = glGetUniformLocation(myShaderProgram, "g_ProjectionMatrix");
	gNormalMatrix = glGetUniformLocation(myShaderProgram, "g_NormalMatrix");


	// Load demo texture...
	myTexture = fiLoadTexture("Resources\\Textures\\Chairwood.jpg");
	myTexture2 = fiLoadTexture("Resources\\Textures\\Stagewood.jpg");
	WallTexture = fiLoadTexture("Resources\\Textures\\wall.png");
	SingerTexture = fiLoadTexture("Resources\\Textures\\singer.png");
	MicStandTexture = fiLoadTexture("Resources\\Textures\\mictand.png");
	// Enable Vertex Arrays
	// Tell OpenGL to expect vertex position information from an array
	glEnableClientState(GL_VERTEX_ARRAY);
	// Tell OpenGL to expect vertex colour information from an array
	glEnableClientState(GL_COLOR_ARRAY);
	// Tell OpenGL to expect vertex colour information from an array
	glEnableClientState(GL_NORMAL_ARRAY);
	// Tell OpenGL to expect texture coordinate information from an array
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	ImportModels();

	cgClock.start();
}

void report_version(void) {

	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";
}

// Update Animated Scene Objects Using Delta Time
void update(void) 
{
	cgClock.tick();
	theta += cgClock.gameTimeDelta();
	//Spinny chair 1
	ChairModel1->rotate(GUQuaternion(0, 0.1, 0.1));
	//Spinny chair 2
	ChairModel2->rotate(GUQuaternion(0, -0.1, -0.1));
	//Spinny chair 3
	ChairModel3->rotate(GUQuaternion(0, -0.1, -0.1));
	//Flying singer stagedive thing
	Singer->rotate(GUQuaternion(0, 0.0, 0.03));
}

// Example rendering functions
void display(void) {

	// Update Animated Scene Objects Using Delta Time
	update();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get camera view and projection transforms
	GUMatrix4 viewMatrix = mainCamera->viewTransform();
	GUMatrix4 projMatrix = mainCamera->projectionTransform();
 
	if (FIXED_FUNCTION)
	{
		glUseProgram(0);

		// set fixed-function projection transform
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf((GLfloat*)&projMatrix);

		// set fixed-function model-view transform
		glMatrixMode(GL_MODELVIEW);
		GUMatrix4 modelViewMatrix = viewMatrix * ChairModel1Matrix;
		glLoadMatrixf((GLfloat*)&modelViewMatrix);

		//glEnable(GL_TEXTURE_2D);

		// Turn on light
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.5);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05);

		// Set Material properties for object
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	}
	else
	{
		// Get Camera Position in World Coordinates
		GUVector4 eyePos = mainCamera->cameraLocation();

		// Get Normal Matrix (transform Normals to World Coordinates)
		GUMatrix4 normalMatrix = ChairModel1Matrix.inverseTranspose();

		// Render using finished shader
		glUseProgram(myShaderProgram);
		glUniform4fv(gDiffuse, 1, (GLfloat*)&light_diffuse);
		glUniform4fv(gAmbient, 1, (GLfloat*)&light_ambient);
		glUniform4fv(gLPosition, 1, (GLfloat*)&light_position);
		glUniform4fv(gEyePos, 1, (GLfloat*)&eyePos);
		GLfloat	attenuation[] = { 1.0, 0.5, 0.05 };

		glUniform3fv(gAttenuation, 1, (GLfloat*)&attenuation);
		glUniformMatrix4fv(gModelMatrix, 1, false, (GLfloat*)&ChairModel1Matrix);
		glUniformMatrix4fv(gViewMatrix, 1, false, (GLfloat*)&viewMatrix);
		glUniformMatrix4fv(gProjectionMatrix, 1, false, (GLfloat*)&projMatrix);
		glUniformMatrix4fv(gNormalMatrix, 1, false, (GLfloat*)&normalMatrix);

		// Material Properties
		// Set Ambient
		glUniform4fv(gMatAmbient, 1, (GLfloat*)&mat_amb_diff);
		// Set Diffuse
		glUniform4fv(gMatDiffuse, 1, (GLfloat*)&mat_amb_diff);

		// The shader requires shininess in w parameter hence:
		GLfloat mat_specular_shader[] = { mat_specular[0], mat_specular[1], mat_specular[2], mat_shininess[0] }; // White highlight
		// Set Specular
		glUniform4fv(gMatSpecular, 1, (GLfloat*)&mat_specular_shader);
	}

	if (ChairModel1)
		ChairModel1->renderTexturedModel();
	if (ChairModel2)
		ChairModel2->renderTexturedModel();
	if (ChairModel3)
		ChairModel3->renderTexturedModel();
	if (Singer)
		Singer->renderTexturedModel();
	if (myModel2)
		myModel2->renderTexturedModel();
	if (myModel3)
		myModel3->renderTexturedModel();
	if (MicStand)
		MicStand->renderTexturedModel();
	if (Wall)
		Wall->renderTexturedModel();
	if (Wall4)
		Wall4->renderTexturedModel();

	glutSwapBuffers();

	glutPostRedisplay(); //Redraw is required for animation
}

#pragma region Importing Models

void ImportModels()
{

	// load stage model
	myModel2 = new CGModel();
	import3DS(L"Resources\\Models\\stage.3DS", myModel2);
	myModel2->setTextureForModel(myTexture2);
	myModel2->translate(GUVector4(70, -15, 0));
	myModel2->scale(GUVector4(3, 2, 3));
	myModelMatrix2 = GUMatrix4::identity();

	// load floor model
	myModel3 = new CGModel();
	import3DS(L"Resources\\Models\\floor.3DS", myModel3);
	myModel3->setTextureForModel(myTexture2);
	myModel3->translate(GUVector4(40, -30, 0));
	myModel3->scale(GUVector4(3, 1, 3));
	myModelMatrix3 = GUMatrix4::identity();

	// load chair model
	ChairModel1 = new CGModel();
	import3DS(L"Resources\\Models\\chair.3DS", ChairModel1);
	ChairModel1->setTextureForModel(myTexture);
	ChairModel1Matrix = GUMatrix4::identity();

	// load chair model
	ChairModel2 = new CGModel();
	import3DS(L"Resources\\Models\\chair.3DS", ChairModel2);
	ChairModel2->setTextureForModel(myTexture);
	ChairModel2->translate(GUVector4(0, 0, 100));
	ChairModel2Matrix = GUMatrix4::identity();

	// load chair model
	ChairModel3 = new CGModel();
	import3DS(L"Resources\\Models\\chair.3DS", ChairModel3);
	ChairModel3->setTextureForModel(myTexture);
	ChairModel3->translate(GUVector4(0, 0, -100));
	ChairModel3Matrix = GUMatrix4::identity();

	//Load mic model
	MicStand = new CGModel();
	import3DS(L"Resources\\Models\\micStand.3DS", MicStand);
	MicStand->setTextureForModel(MicStandTexture);
	MicStand->translate(GUVector4(140, -10, 0));
	micStandMatrix = GUMatrix4::identity();

	//Load singer model
	Singer = new CGModel();
	import3DS(L"Resources\\Models\\singer.3DS", Singer);
	Singer->setTextureForModel(SingerTexture);
	Singer->translate(GUVector4(170, -10, 0));
	singerMatrix = GUMatrix4::identity();

	//Load wall models
	Wall = new CGModel();
	import3DS(L"Resources\\Models\\wall.3DS", Wall);
	Wall->setTextureForModel(WallTexture);
	Wall->translate(GUVector4(21, -10, -210));
	Wall->scale(GUVector4(5.3, 4, 1));
	wallMatrix = GUMatrix4::identity();

	Wall2 = new CGModel();
	import3DS(L"Resources\\Models\\wall.3DS", Wall2);
	Wall2->setTextureForModel(WallTexture);
	Wall2->translate(GUVector4(21, -10, 210));
	Wall2->scale(GUVector4(5.3, 4, 1));
	wall2Matrix = GUMatrix4::identity();

	Wall3 = new CGModel();
	import3DS(L"Resources\\Models\\wall2.3DS", Wall3);
	Wall3->setTextureForModel(WallTexture);
	Wall3->translate(GUVector4(-88, -10, 5));
	Wall3->scale(GUVector4(1, 4, 5.3));
	wall3Matrix = GUMatrix4::identity();

	Wall4 = new CGModel();
	import3DS(L"Resources\\Models\\wall2.3DS", Wall4);
	Wall4->setTextureForModel(WallTexture);
	Wall4->translate(GUVector4(-88, -10, 5));
	Wall4->scale(GUVector4(1, 4, 5.3));
	wall4Matrix = GUMatrix4::identity();


}
#pragma endregion

#pragma region Event handling functions

void mouseButtonDown(int button_id, int state, int x, int y) {

	if (button_id == GLUT_LEFT_BUTTON) {

		if (state == GLUT_DOWN) {

			mouse_x = x;
			mouse_y = y;

			mDown = true;

		}
		else if (state == GLUT_UP) {

			mDown = false;
		}
	}
}


void mouseMove(int x, int y) {

	if (mDown) {

		int dx = x - mouse_x;
		int dy = y - mouse_y;

		//theta += (float)dy * (3.142f * 0.01f);

		// rotate camera around the origin
		if (mainCamera)
			mainCamera->transformCamera((float)-dy, (float)-dx, 0.0f);

		mouse_x = x;
		mouse_y = y;

		glutPostRedisplay();
	}
}


void mouseWheelUpdate(int wheel_number, int direction, int x, int y) {

	if (mainCamera) {

		if (direction<0)
			mainCamera->scaleCameraRadius(1.1f);
		else if (direction>0)
			mainCamera->scaleCameraRadius(0.9f);

		glutPostRedisplay();
	}
}


void keyDown(unsigned char key, int x, int y) {

	if (key == 'r') {

		theta = 0.0f;
		glutPostRedisplay();
	}
}

#pragma endregion
