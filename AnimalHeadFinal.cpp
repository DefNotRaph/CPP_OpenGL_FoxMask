/* Student		: Raphael Rose
 * Assignment	: 5-1
 * Date			: Dec. 6, 2020
 *
 */
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

 // GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL2/SOIL2.h>

using namespace std;

int width, height;
const double PI = 3.14159;
const float toRadians = PI / 180.0f;

//Declare View Matrix
glm::mat4 viewMatrix;
//Declare Projection Matrix
glm::mat4 projectionMatrix;

//Initialize FOV
GLfloat fov = 45.f;

//Define Camera Attributes
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 targetPosition = glm::vec3(0.0f, 0.0f, 0.0f); //Define Target
glm::vec3 cameraDirection = glm::normalize(cameraPosition - targetPosition);
glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f); //Define World Up Vector
glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
glm::vec3 cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));

//Declare target prototype
glm::vec3 getTarget();

// Camera transformation prototype
void transformCamera();

// Boolean arrays for keys and mouse buttons
bool keys[1024], mouseButtons[3];

// Boolean to check camera transformation
bool isPanning = false, isOrbiting = false, isZooming = false, isPerspective = false;

// Radius, Pitch, and Yaw
GLfloat radius = 3.f, rawPitch = 0.f, rawYaw = 0.f, degYaw, degPitch;

GLfloat deltaTime = 0.0f, lastFrame = 0.0f;
GLfloat lastX = 320, lastY = 240, xChange, yChange;

bool firstMouseMove = true; // Detect initial mouse movement

void initCamera();	//resets cameral position
void changeMode();	//changes from ortho to perspective
void getMode();

//input callback function prototypes

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

//Light source position
glm::vec3 lightPosition(1.0f, 1.0f, 1.0f);

// Draw Primitive(s)
void draw()
{
	GLenum mode = GL_TRIANGLES;
	GLsizei indices = 56 * 3;
	glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);


}

// Create and Compile Shaders
static GLuint CompileShader(const string& source, GLuint shaderType)
{
	// Create Shader object
	GLuint shaderID = glCreateShader(shaderType);
	const char* src = source.c_str();

	// Attach source code to Shader object
	glShaderSource(shaderID, 1, &src, nullptr);

	// Compile Shader
	glCompileShader(shaderID);

	// Return ID of Compiled shader
	return shaderID;

}

// Create Program Object
static GLuint CreateShaderProgram(const string& vertexShader, const string& fragmentShader)
{
	// Compile vertex shader
	GLuint vertexShaderComp = CompileShader(vertexShader, GL_VERTEX_SHADER);

	// Compile fragment shader
	GLuint fragmentShaderComp = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

	// Create program object
	GLuint shaderProgram = glCreateProgram();

	// Attach vertex and fragment shaders to program object
	glAttachShader(shaderProgram, vertexShaderComp);
	glAttachShader(shaderProgram, fragmentShaderComp);

	// Link shaders to create executable
	glLinkProgram(shaderProgram);

	// Delete compiled vertex and fragment shaders
	glDeleteShader(vertexShaderComp);
	glDeleteShader(fragmentShaderComp);

	// Return Shader Program
	return shaderProgram;

}


int main(void)
{
	width = 640; height = 480;

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Main Window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Set input callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
		cout << "Error!" << endl;

	GLfloat vertices[] = {

		// Triangle 1
		0.0, 0.0, 0.2, // index 0
		1.0, 0.6, 0.0, // orange
		0.539, 1.0-0.523,//UV
		-0.00964f, 0.02055f, 0.00918f,

		0.0, 0.4, 0.0, // index 1
		1.0, 0.0, 0.0, // red
		0.52, 1.0-0.75,
		-0.0154f, 0.f, 0.f,

		-0.33, 0.6, -0.2, // index 2
		1.0, 0.0, 0.0, // red
		0.24, 1.0-0.87,
		0.00133f, -0.03028f, -0.02972f,

		-0.5, 0.8, -0.1, // index 3
		1.0, 1.0, 1.0, // white
		0.04, 1.0-0.98,
		0.02f, -0.0125f, 0.059f,

		-0.625, 0.6, -0.1, // index 4
		1.0, 0.0, 0.0, // red
		0.016, 1.0-0.799,
		0.023f, -0.01283f, -0.04742f,

		-0.125, 0.375, -0.02, // index 5
		1.0, 0.0, 0.0, // red
		0.357, 1.0-0.711,
		-0.00113f, -0.0072f, -0.00065f,

		-0.25, 0.365, 0.0, // index 6
		1.0, 0.6, 0.0, // orange
		0.289, 1.0 - 0.658,
		0.0056f, -0.0136f, -0.0222f,

		-0.14, 0.26, 0.0, // index 7
		1.0, 0.6, 0.0, // orange
		0.341, 1.0 - 0.577,
		-0.00534f, -0.0038f, -0.00264f,

		-0.4, 0.25, 0.0, // index 8
		1.0, 0.6, 0.0, // orange
		0.179, 1.0 - 0.587,
		-0.00388f, -0.0145f, -0.02204f,

		-0.59, 0.11, -0.2, // index 9
		1.0, 0.0, 0.0, // red
		0.088, 1.0 - 0.534,
		0.01633f, 0.00633f, -0.02899f,

		-0.5, -0.025, 0.0, // index 10
		1.0, 0.6, 0.0, // orange
		0.172,1.0 - 0.423,
		-0.01357f, -0.00046f, 0.02156f,


		-0.635, -0.125, 0.0, // index 11
		1.0, 1.0, 1.0, // white
		0.042,1.0 - 0.379,
		-0.0025f, 0.00337f, 0.01576f,


		-0.375, -0.1, 0.0, // index 12
		1.0, 0.0, 0.0, // red
		0.276,1.0 - 0.349,
		0.00875f, -0.00812f, -0.00531f,


		-0.23, -0.125, 0.2, // index 13
		1.0, 0.0, 0.0, // red
		0.383, 0.285,
		-0.0025f, 0.00625f, 0.00357f,


		-0.355, -0.2, 0.05, // index 14
		1.0, 0.0, 0.0, // red
		0.276, 0.279,
		-0.01062f, -0.006f, -0.00006f,


		-0.25, -0.275, 0.25, // index 15
		1.0, 1.0, 1.0, // white
		0.377,1.0 - 0.168,
		-0.03438f, 0.00837f, 0.02606f,


		-0.1, -0.25, 0.6, // index 16
		0.7, 0.4, 0.0, // light brwn
		0.581,1.0 - 0.205,
		-0.00125f, 0.03695f, 0.02248f,


		-0.145, -0.275, 0.6, // index 17
		1.0, 1.0, 1.0, // white
		0.503,1.0 - 0.138,
		0.00344f, 0.00394f, 0.0025f,


		0.0, -0.45, 0.65, // index 18
		0.7, 0.4, 0.0, // light brwn
		0.64,1.0 - 0.104,
		-0.0025f, 0.0067f, -0.00945f,


		0.0, -0.425, 0.55, // index 19
		0.7, 0.4, 0.0, // light brwn
		0.64,1.0 - 0.05,
		0.0f, 0.02175f, -0.00363f,


		0.33, 0.6, -0.2, // index 20
		1.0, 0.0, 0.0, // red 
		0.74,1.0 - 0.896,
		0.01883f, 0.00428f, -0.02191f,


		0.5, 0.8, -0.1, // index 21
		1.0, 1.0, 1.0, // red
		0.932,1.0 - 0.98,
		0.02f, 0.0125f, -0.059f,


		0.625, 0.6, -0.1, // index 22
		1.0, 0.0, 0.0, // red
		0.942,1.0 - 0.805,
		0.04633f, -0.02183f, -0.02142f,


		0.125, 0.375, -0.02, // index 23
		1.0, 0.0, 0.0, // red
		0.614,1.0 - 0.775,
		-0.00113f, 0.0072f, 0.00065f,


		0.25, 0.365, 0.0, // index 24
		1.0, 0.6, 0.0, // orange
		0.659, 1.0 - 0.708,
		0.0056f, 0.0136f, 0.0222f,


		0.14, 0.26, 0.0, // index 25
		1.0, 0.6, 0.0, // orange
		0.591, 1.0 - 0.644,
		-0.00614f, -0.017f, -0.02496f,


		0.4, 0.25, 0.0, // index 26
		1.0, 0.6, 0.0, // orange
		0.75, 1.0 - 0.668,
		0.00437f, -0.0115f, -0.02102f,

		0.59, 0.11, -0.2, // index 27
		1.0, 0.0, 0.0, // red
		0.87, 1.0 - 0.564,
		0.01633f, -0.00633f, 0.02899f,

		0.5, -0.025, 0.0, // index 28
		1.0, 0.6, 0.0, // orange
		0.821, 1.0 - 0.456,
		-0.01357f, 0.00046f, -0.02156f,

		0.635, -0.125, 0.0, // index 29
		1.0, 1.0, 1.0, // white
		0.912, 1.0 - 0.386,
		-0.0025f, -0.00337f, -0.01576f,

		0.375, -0.1, 0.0, // index 30
		1.0, 0.0, 0.0, // red
		0.756, 1.0 - 0.413,
		-0.00125f, -0.00338f, -0.00338f,


		0.23, -0.125, 0.2, // index 31
		1.0, 0.0, 0.0, // red
		0.672, 1.0 - 0.356,
		0.00964f, 0.02004f, 0.01418f,


		0.355, -0.2, 0.05, // index 32
		1.0, 0.0, 0.0, // red
		0.756, 1.0 - 0.289,
		-0.01062f, 0.006f, 0.00006f,


		0.25, -0.275, 0.25, // index 33
		1.0, 1.0, 1.0, // white
		0.734, 1.0 -  0.235,
		-0.03438f, -0.00837f, -0.02606f,

		0.1, -0.25, 0.6, // index 34
		0.7, 0.4, 0.0, // light brwn
		0.679, 1.0 - 0.201,
		0.02375f, 0.04505f, 0.02533f,

		0.145, -0.275, 0.6, // index 35
		1.0, 1.0, 1.0, // white
		0.737, 1.0 - 0.131,
		-0.00281f, 0.00694f, -0.00431f,

		-0.26, 0.0, 0.0, // index 36
		0.0, 0.0, 0.0, // black
		0.38, 1.0 - 0.46,
		-0.001f, -0.0194f, 0.01995f,

		0.26, 0.0, 0.0, // index 37
		0.0, 0.0, 0.0, // black
		0.698, 1.0 - 0.48,
		-0.009f, 0.0102f, -0.0269f
	};

	// Define element indices
	GLubyte indices[] = {
		0,1,25,
		0,1,7,
		0,8,7,
		0,8,36,
		0,16,34,
		0,13,16,
		0,34,31,
		0,31,37,
		0,26,37,
		0,25,26,
		0,13,36,
		1,5,7,
		1,23,25,
		2,5,6,
		2,6,8,
		2,8,4,
		2,3,4,
		4,8,9,
		5,6,7,
		6,7,8,
		8,9,10,
		8,10,36,
		9,10,11,
		10,11,15,
		10,12,14,
		10,14,15,
		10,12,36,
		12,36,13,
		12,13,14,
		13,14,15,
		13,15,17,
		13,16,17,
		16,17,18,
		16,34,18,
		17,18,19,
		18,35,19,
		18,34,35,
		20,21,22,
		20,23,24,
		20,24,26,
		20,22,26,
		22,26,27,
		23,24,25,
		24,25,26,
		26,27,28,
		26,28,37,
		27,28,29,
		28,29,33,
		28,30,32,
		28,30,37,
		28,32,33,
		30,31,37,
		30,31,32,
		31,32,33,
		31,33,35,
		31,34,35

	};

	// Plane Transforms
	glm::vec3 planePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.5f),
		glm::vec3(0.5f,  0.0f,  0.0f),
		glm::vec3(0.0f,  0.0f,  -0.5f),
		glm::vec3(-0.5f, 0.0f,  0.0f)
	};

	glm::float32 planeRotations[] = {
		0.0f, 90.0f, 0.0f, 90.0f
	};

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint VBO, EBO, VAO;

	glGenBuffers(1, &VBO); // Create VBO
	glGenBuffers(1, &EBO); // Create EBO

	glGenVertexArrays(1, &VAO); // Create VOA
	glBindVertexArray(VAO);

	// VBO and EBO Placed in User-Defined VAO
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Select VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Select EBO


	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Load vertex attributes
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

 // Specify attribute location and layout to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)

	//Load textures
	int texWidth, texHeight;
	unsigned char* image = SOIL_load_image("FoxFace.png", &texWidth, &texHeight, 0, SOIL_LOAD_RGB);

	//Generate textures
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);


	// Vertex shader source code
	string vertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"
		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"out vec3 oNormal;"
		"out vec3 fragPos;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"oNormal =  mat3(transpose(inverse(model))) * normal;"
		"fragPos = vec3(model * vec4(vPosition.x, vPosition.y, vPosition.z , 1.0f));"
		"}\n";

	// Fragment shader source code
	string fragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"in vec3 oNormal;"
		"in vec3 fragPos;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"uniform vec3 objectColor;"
		"uniform vec3 lightColor;"
		"uniform vec3 lightPos;"
		"uniform vec3 viewPos;"
		"void main()\n"
		"{\n"
		"//Ambient\n"
		"float ambientStrength = 0.1f;"
		"vec3 ambient = ambientStrength * lightColor;"
		"//Diffuse\n"
		"vec3 norm = normalize(oNormal);"
		"vec3 lightDir = normalize(lightPos - fragPos);"
		"float diff = max(dot(norm, lightDir),0.0);"
		"vec3 diffuse = diff + lightColor;"
		"//Specular\n"
		"float specularStrength = 1.5f;"
		"vec3 viewDir = normalize(viewPos - fragPos);"
		"vec3 reflectDir = reflect(-lightDir, norm);"
		"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);"
		"vec3 specular = specularStrength * spec * lightColor;"
		"vec3 result = (ambient + diffuse + specular) * objectColor;"
		"fragColor = texture(myTexture, oTexCoord) * vec4(result, 1.0f);"
		"}\n";

	// Creating Shader Program
	GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// Set delta time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Resize window and graphics simultaneously
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use Shader Program exe and select VAO before drawing 
		glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes

		getMode();

		// Declare transformations (can be initialized outside loop)		

		glm::mat4 modelMatrix;

		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
		//modelMatrix = glm::rotate(modelMatrix, -60.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//modelMatrix = glm::rotate(modelMatrix, -15.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

		viewMatrix = glm::lookAt(cameraPosition, getTarget(), worldUp);


		// Get matrix's uniform location and set matrix
		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");


		//Get light and object color, and light position location
		GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

		//Assign colors
		glUniform3f(objectColorLoc, 0.46f, 0.36f, 0.25f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);


		//Assign light position
		glUniform3f(lightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(viewPosLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO); // User-defined VAO must be called before draw. 



		// Draw primitive(s)
		draw();


		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after
		glUseProgram(0); // Incase different shader will be used after

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		/* Poll camera transformations */
		transformCamera();
	}

	//Clear GPU resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);


	glfwTerminate();
	return 0;
}

//define Input callback functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//Display ASCII Keycode
	cout << "ASCII: " << key << endl;

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	//Display  Scroll Offset
	if (yoffset > 0)
		cout << "Scroll Up: " << fov << endl;
	if (yoffset < 0)
		cout << "Scroll Down: " << fov << endl;

	if (fov >= 1.f && fov <= 56.f)
		fov += yoffset * 0.01f;

	if (fov > 55.f)
		fov = 55.f;
	if (fov < 1.f)
		fov = 1.f;


}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {

	//Display the Cursor Offset
	//cout << "Mouse X: " << xpos << endl;
	//cout << "Mouse Y: " << ypos << endl;

	if (firstMouseMove) {
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}

	//calculate the mouse cursor offset
	xChange = xpos - lastX;
	yChange = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	//Pan Camera
	if (isPanning) {
		if (cameraPosition.z < 0.f)
			cameraFront.z = 1.f;
		else
			cameraDirection.z = -1.f;

		GLfloat cameraSpeed = xChange * deltaTime;
		cameraPosition += cameraSpeed * cameraRight;

		cameraSpeed = yChange * deltaTime;
		cameraPosition += cameraSpeed * cameraUp;
	}

	if (isOrbiting) {
		rawYaw += xChange;
		rawPitch += yChange;

		degYaw = glm::radians(rawYaw);
		degPitch = glm::clamp(glm::radians(rawPitch), -glm::pi<float>() / 2.f + .1f, glm::pi<float>() / 2.f - .1f);

		//azimuth altitude formula
		cameraPosition.x = targetPosition.x + radius * cosf(degPitch) * sinf(degYaw);
		cameraPosition.y = targetPosition.y + radius * sinf(degPitch);
		cameraPosition.z = targetPosition.z + radius * cosf(degYaw) * cosf(degPitch);
	}

	if (isZooming) {
		// Clamp FOV
		if (fov >= 1.0f && fov <= 55.0f)
			fov -= yChange * 0.01;

		// Default FOV
		if (fov < 1.0f)
			fov = 1.0f;
		if (fov > 55.0f)
			fov = 55.0f;
	}


}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	//Detect mouse button clicks
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		cout << "LMB CLICKED" << endl;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
		cout << "MMB CLICKED" << endl;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		cout << "RMB CLICKED" << endl;

	if (action == GLFW_PRESS)
		mouseButtons[button] = true;
	else if (action == GLFW_RELEASE)
		mouseButtons[button] = false;

}

//Define get target function
glm::vec3 getTarget() {
	if (isPanning)
		targetPosition = cameraPosition + cameraFront;
	return targetPosition;
}

//Define transform camera function
void transformCamera()
{
	// Pan camera
	if (keys[GLFW_KEY_LEFT_ALT] && mouseButtons[GLFW_MOUSE_BUTTON_MIDDLE])
		isPanning = true;
	else
		isPanning = false;

	//orbit camera
	if (keys[GLFW_KEY_LEFT_ALT] && mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
		isOrbiting = true;
	else
		isOrbiting = false;

	//zoom camera
	if (keys[GLFW_KEY_LEFT_ALT] && mouseButtons[GLFW_MOUSE_BUTTON_RIGHT])
		isZooming = true;
	else
		isZooming = false;


	//reset camera
	if (keys[GLFW_KEY_F])
		initCamera();

	//toggle perspective
	if (keys[GLFW_KEY_P])
		changeMode();
}

void initCamera() {
	//Define Camera Attributes
	cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
	targetPosition = glm::vec3(0.0f, 0.0f, 0.0f); //Define Target
	cameraDirection = glm::normalize(cameraPosition - targetPosition);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f); //Define World Up Vector
	cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
	cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
	cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));

}

void changeMode() {
	if (isPerspective) {
		projectionMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);
		isPerspective = false;
	}
	else {
		projectionMatrix = glm::perspective(fov, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
		isPerspective = true;
	}
}


void getMode() {
	if (isPerspective) {
		projectionMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);
	}
	else {
		projectionMatrix = glm::perspective(fov, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	}
}
