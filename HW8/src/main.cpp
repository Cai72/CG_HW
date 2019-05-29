#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>

#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void cursor_position_callback(GLFWwindow* window, double x, double y);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;
typedef struct Point2D {
	float x;
	float y;
	float z;
	void set(float x, float y, float z=0) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int controlvertexNum = 0;
const int maxcontrolNum = 10;
const int curveVetexTotalNum = 400;
Point2D  controlVertices[maxcontrolNum];
float  vertices[(curveVetexTotalNum + maxcontrolNum) * 3] = { 0 };

//mouse
float mx;
float my;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	//glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader lightingShader("7.4.camera.vs", "7.4.camera.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();

		// first, configure the cube's VAO (and VBO)
		unsigned int VBO, cubeVAO;
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*((curveVetexTotalNum + controlvertexNum) * 3), vertices, GL_STATIC_DRAW);

		glBindVertexArray(cubeVAO);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// render the cube
		glBindVertexArray(cubeVAO);
		if (controlvertexNum > 0) {
			glDrawArrays(GL_POINTS, 0, curveVetexTotalNum+controlvertexNum);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
		glDeleteVertexArrays(1, &cubeVAO);
		glDeleteBuffers(1, &VBO);
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}
double stairsMultiply(int n) {
	if (n == 0||n == 1)
		return 1;
	int result = 1;
	for (int i = 2; i <= n; i++) {
		result *= i;
	}
	return double(result);
}
double m_pow(float base, int exp) {
	if (exp == 0) {
		return 1;
	}
	else {
		return pow(base, exp);
	}
}
double Bernstein(int i, int n, float t) {
	return (stairsMultiply(n) / stairsMultiply(i) / stairsMultiply(n - i)) * m_pow(t, i) * m_pow(1 - t, n - i);
}
void updateCurveVertex() {
	for (int i = 0; i < curveVetexTotalNum; i++) {
		double t = (1/float(curveVetexTotalNum)) * i;
		double x=0,y=0;
		for (int j = 0; j < controlvertexNum; j++) {
			double b = Bernstein(j,controlvertexNum-1,t);
			x += double(controlVertices[j].x) * b;
			y += double(controlVertices[j].y) * b;
			//std::cout << ("Iteration position:(") << x << ", " << y << ")" << std::endl;
		}
		vertices[i * 3] = float(x);
		vertices[i * 3 + 1] = float(y);
		vertices[i * 3 + 2] = 0.0f;
		std::cout << ("Final vertex position:(") << x << ", " << y << ")" << std::endl;
	}
	for (int i = 0; i < controlvertexNum; i++) {
		vertices[curveVetexTotalNum + i * 3] = controlVertices[i].x;
		vertices[curveVetexTotalNum + i * 3 + 1] = controlVertices[i].y;
		vertices[curveVetexTotalNum + i * 3 + 2] = controlVertices[i].z;
	}
}
void addControlVertex() {
	//std::cout<<("add")<<std::endl;
	//std::cout << ("Mouse position:(") << mx << ", " << my << ")" << std::endl;
	if (controlvertexNum < maxcontrolNum) {
		controlVertices[controlvertexNum++].set(mx,my);
		updateCurveVertex();
	}
}
void deleteControlVertex() {
	//std::cout << ("delete") << std::endl;
	//std::cout << ("Mouse position:(") << mx << ", " << my << ")" << std::endl;
	if (controlvertexNum > 0) {
		controlvertexNum--;
		updateCurveVertex();
	}
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
	mx = float((x-SCR_WIDTH/2)/(SCR_WIDTH/2));
	my = float(-(y - SCR_HEIGHT / 2) / (SCR_HEIGHT / 2));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		addControlVertex();
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		deleteControlVertex();
		break;
	default:
		return;
	}
	return;
}