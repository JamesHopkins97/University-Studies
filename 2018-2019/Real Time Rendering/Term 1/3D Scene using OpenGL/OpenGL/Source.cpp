#include "Includes.h"
#include "VertexData.h"
#include "TableScene.h"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

const int screenHeight = 900;
const int screenWidth = 1600;

// Camera settings
//							  width, heigh, near plane, far plane
Camera_settings camera_settings{ screenWidth, screenHeight, 0.1, 100.0 };

//Timer
Timer timer;

// Instantiate the camera object with basic data
Camera camera(camera_settings, glm::vec3(0.0f, 0.5f, 4.0f));

double lastX = camera_settings.screenWidth / 2.0f;
double lastY = camera_settings.screenHeight / 2.0f;

bool			showTableQuad = false;
TableScene		*tableScene = nullptr;


int main()
{

#pragma region WindowSetup
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 16);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(camera_settings.screenWidth, camera_settings.screenHeight, "Hello Marius. Please give me 1st <3", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the callback functions
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;

	}
#pragma endregion

	//Rendering settings
	glfwSwapInterval(0);		// glfw enable swap interval to match screen v-sync
	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); //Enables face culling
	glFrontFace(GL_CCW);//Specifies which winding order if front facing

	//
	// Table scene
	//
	TexturedQuad	*tableQuad = nullptr;
	TexturedQuad	*texturedQuad = nullptr;
	bool			leftCtrlPressed = false;

	int samples = 8;
	tableScene = new TableScene(screenWidth, screenHeight, samples);
	texturedQuad = new TexturedQuad(string("Resources\\Models\\Blue Marble\\clouds.bmp"));
	tableQuad = new TexturedQuad(tableScene->getTableSceneTexture(), true, true, screenWidth, screenHeight, samples);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);
		timer.tick();

		//
		// Pass 1. Render the Table scene
		//
		tableScene->render();


		//
		// Pass 2. Render the basic demo scene to the screen
		//

		// Clear the screen
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Reset the viewport
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		if (showTableQuad)
		{
			if (tableQuad)
				tableQuad->render();
		}
		else
		{
			if (texturedQuad)
				texturedQuad->render();
		}

		//string title = "FPS: " + std::to_string(timer.averageFPS()) + " SPF: " + std::to_string(timer.currentSPF());
		//glfwSetWindowTitle(window, title.c_str());



		// glfw: swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

#pragma region Controls

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	timer.updateDeltaTime();
	Camera *ecam = tableScene->getTableSceneCamera();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		ecam->processKeyboard(FORWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		ecam->processKeyboard(BACKWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		ecam->processKeyboard(LEFT, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		ecam->processKeyboard(RIGHT, timer.getDeltaTimeSeconds());
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		showTableQuad = !showTableQuad;
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	glViewport(0, 0, width, height);
	camera.updateScreenSize(width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	Camera *ecam = tableScene->getTableSceneCamera();
	ecam->processMouseMovement(xoffset, yoffset);

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Camera *ecam = tableScene->getTableSceneCamera();
		ecam->processMouseScroll(yoffset);
	}
	else
	{
		camera.processMouseScroll(yoffset);
	}
}
#pragma endregion
