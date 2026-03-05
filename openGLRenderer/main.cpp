//glad is a library that manages function pointers for openGL, so we include it before glfw
#include <glad/glad.h>
//glfw is a library that handles window creation and input for openGL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm_test.h"
#include <stb_image.h>
#include <iostream>
#include<fstream>
#include <sstream>
#include<string>
#include "shader.h"
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Camera* camera, float deltaTime);
//function which handles keyboard input for single press events like toggling wireframe mode and flipping textures, since those don't need to be checked every frame in processInput
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
//function which handles mouse input when movement detected
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//function which handles scroll input when scroll detected
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//Check the equality of two floats with a given epsilon, to account for precision errors in floating point numbers
bool floatEquality(float f1, float f2, float epsilon = 1e-6f);

/*
* NOTE: Don't need this anymore since we made a shader class that can read from file
* 
//the source code for our vertex shader we will compile later
std::string vertexShaderString = R"(
#version 330 core
layout (location = 0) in vec3 aPos; // position has attribute positon 0
layout (location = 1) in vec3 aColor; //color has attribute position 1

out vec4 ourColor; // specify a color output to the fragment shader

void main() {
	gl_Position = vec4(aPos, 1.0); // we give vec3 to vec4's constructor
	ourColor = vec4(aColor, 1.0); //output variable to dark-red
})";
//the source code for the fragment shader, this one always outputs an orange
std::string fragmentShaderString = R"(
#version 330 core
out vec4 FragColor;

in vec4 ourColor; // input variable from vertex shader

void main() {
	FragColor = ourColor;
})";
const char* vertexShaderSource = vertexShaderString.c_str();
const char* fragmentShaderSource = fragmentShaderString.c_str();
*/
int wireframe_mode = 0;
int freefly_mode = 0;
int flip_mode = 0;
int respond_to_flip = 0;
int respond_to_mix_change = 0;
//Defines the percentage mix of our two textures. 0 being all of the color from texture 1 
// and 1 being all of the color from texture 2
float mix_percentage = 0.2f;


const int WIDTH = 800;
const int HEIGHT = 600;

int main() {


	//Define Camera
	Camera camera;

	//vertices of our triangles to render:
	//remember these are defined in local space
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};


	// world space positions of our cubes
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	/*
	* INITIALIZE OPENGL WINDOW
	*/
	glfwInit();
	//First argument is what option we want to configure, the second is the value of that option we want to set
	//These first 2 calls to window hint set the open GL version to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//This call sets the OpenGL profile to core, which means we only have access to modern functions, and not deprecated ones.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//this sets our window as the main context of our current thread?
	glfwMakeContextCurrent(window);
	//attach the camera to our window, so we can access it in glfw call back functions
	glfwSetWindowUserPointer(window, &camera);
	//set key callback to handle single press inputs like toggling wireframe mode and flipping textures
	glfwSetKeyCallback(window, key_callback);
	//set our mouse input mode, hide the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//set our callback function
	glfwSetCursorPosCallback(window, mouse_callback);
	//set our scroll callback function
	glfwSetScrollCallback(window, scroll_callback);


	//initialize GLAD function pointers for openGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD, I am SAD : (" << std::endl;
		return -1;
	}

	//enable z buffering, otherwise our cube rendering will look messed up because some triangles will render on top of ones their behind
	glEnable(GL_DEPTH_TEST);


	//make a vertex array object too (research this more)
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	//create vertex buffer object to pass vertex data to our gpu
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	//if an access violation occurs, it's because glad is trying to call glad_glbindbuffer, but there is no context for it to call the function to. Just make sure the window and glad stuff are handled before this part
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/*
	//create an EBO
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	*/

	//test if our shader class worked!
	Shader ourShader("vertex.glsl", "fragment.glsl");
	//Tell open gl how to read our vertex data
	/* arguments
	* 1st: the vertex attribute we want to configure, the position was specified with the shader (layout (location = 0))
	* 2nd: the size of the vertex attribute
	* 3rd: the type of the data, in this case a float
	* 4th: they next argument specifies if we want the data to be normalized, set to false since we defined vertices in the (-1.0, 1.0) range
	* 5th: The stride, space between consectutive vertex attributes, this is byte distance to the next value of the SAME attribute (i.e, shift forward to the same spot in the NEXT vertex), 8 * sizeof(float), would take us to the Pos attribute of the NEXT vertex
	* (assuming we're discussing attribute 0), this is why the stride is the same for all 3 attribs
	* 6th: offset  of where the position data begins the buffer. Since the position data starts at the beginning of the area, just 0, for the next attribute we shift over by 3 since the color data starts after the position data
	and for the texture coords we shift over by 6 since they start after the position and color data

	so the memory layout for one vertex looks like this

	X, Y, Z, R, G, B, S, T
	*/
	//setting position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//setting texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//give opengl the size of our rendering window, so it scales data and coordinates with respect to the window
	// 0, 0 are the x, y coordinates of the start of the window, 800, 600 is the width and height
	glViewport(0, 0, 800, 600);
	

	//have the window and viewport fix itself with a callback function each time the window is resized
	//The second argument is the function we want to pass to glfw to handle the window resizing
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//load an object texture
	int w, h, nrChannels;

	//Before we load in our textures, they render upside down, so we will set this function from stb_image.h to be true,
	//They flip vertically because OpenGL expects 0.0 on the y-axis to be at the bottom of the image, but images usually have it at the top, so this just accounts for that by
	// flipping the image when it's loaded in.
	stbi_set_flip_vertically_on_load(true);
	//pass in path, width, height, and number of color channels
	unsigned char* data = stbi_load("container.jpg", &w, &h, &nrChannels, 0);
	//textures like any opengl object has an id
	unsigned int texture1;
	//first argument is number of textures, second is an int array of id's we want to store them in, in our case 1
	glGenTextures(1, &texture1);
	//bind the texture so any texture commands apply to this one
	glBindTexture(GL_TEXTURE_2D, texture1);
	//set wrapping and filtering options for our currently bound texture
	//texture wrapping options
	//GL_REPEAT is what causes the textures to repeat when you set the texture coordinates out of range
	// (for example 0.0-2.0 vs 0.0-1.0)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//Texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//time to generate our texture, long function explanation coming up
	/*
	* 1st arg: the texture target, so this will affect all bound 2d texture targets
	* 2nd arg: mipmap level to create for the texture, in this case we will not set a manual mipmap level
	* 3rd arg what format to store the texture, since our image only uses rgb we will store it as rgb
	* 4th arg: width of the resulting texture
	* 5th arg: height of the resulting texture
	* 6th arg: always 0 (legacy stuff)
	* 7th arg: format of the source image
	* 8th arg: datatype of the source image (unsigned char* data, which is just unsigned bytes)
	* 9th arg: the actual image data
	*/
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//generates mipmap to bound 2d texture (obviously)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "failed to load texture 1" << std::endl;
	}
	// free our image memory now that we generated the texture
	stbi_image_free(data);
	unsigned int texture2;


	//first argument is number of textures, second is an int array of id's we want to store them in, in our case 1
	glGenTextures(1, &texture2);
	//bind the texture so any texture commands apply to this one
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//mipmap options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("awesomeface.png", &w, &h, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//generates mipmap to bound 2d texture (obviously)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "failed to load texture 2" << std::endl;
	}
	stbi_image_free(data);
	ourShader.use();
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture2"), 1);

	//matrices sanity check from glm_test.cpp
	test_matrices();
	/*example frustum definition
	first two parameters specify left and right coordinate of the frustum
	third and forth specify top and bottom parts
	fifth and sixth specify the distances between the near and far plane (since we defined the planes size with the first four)
	this will transform all coordinates within the frustum to normalized device coordinates (NDC)
	
	*/
	//glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
	float currentFrame = 0.0f; //time of current frame
	float deltaTime = 0.0f;	//time between current frame and last frame
	float lastFrame = 0.0f; //time of last frame
	/*
	* RENDER LOOP, each interation of this following loop is a frame
	*/	
	while (!glfwWindowShouldClose(window)) {
		//calculate frame logic
		currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window, &camera, deltaTime);
		//RENDER COMMANDS START
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//Clear both the color buffer AND the depth buffer now
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		ourShader.use();
		/*
		perspective projection matrix definition
		creates a large frustum that defines visible space,
		first param is fov, second is aspect ratio,
		third and forth are once again our distances of the near and far plane
		if we set near to something too big, you might get that result like in some video games where the camera
		clips through objects too close.

		We use perspective over ortho since ortho won't divide by each 4d vector's w component,
		this means objects won't look smaller as they get farther away
		*/
		glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

		glm::mat4 view;
		view = camera.getView();

		/*
		tying this all together, we should get something like this
		view_clipped = M_project * M_view * M_model * local_coords
		remember matrix operations are read right to left
		*/
		int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(ourShader.ID, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; i++) {
			/*model matrix definition
			we'll define the model matrix as the identity matrix to start, then apply a rotation matrix to it.
			*/
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			//ensures a different angle for each cube
			float angle = 20.0f * i;
			if (i % 3 == 0) {
				angle = angle + glfwGetTime() * 20.0f;
			}
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			int modelLoc = glGetUniformLocation(ourShader.ID, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//RENDER COMMANDS END
		//poll events checks if any events (like the window being resized) triggered and calls their corresponding function
		glfwPollEvents();
		if (respond_to_flip) {
			if (flip_mode) {
				ourShader.setBool("flip", true);

			}
			else {
				ourShader.setBool("flip", false);
			}
			respond_to_flip = 0;
		}
		//swaps the buffer
		glfwSwapBuffers(window);
		if (respond_to_mix_change) {
			ourShader.setFloat("mix_percentage", mix_percentage);
		}
	}

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
	//properly clean up and delete when the render loop ends
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}
//sets the viewport of opengl to fit with our current window size
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


//function for handling our input
//We will pass a reference to camera so processInput modifies our camera's position within the function
void processInput(GLFWwindow* window, Camera* camera, float deltaTime) 
{
	float cameraSpeed = static_cast<float>(2.5 * deltaTime);
	glm::vec3 front = camera->cameraFront;
	glm::vec3 up = camera->cameraUp;
	if (freefly_mode) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->cameraPos += cameraSpeed * front;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->cameraPos -= cameraSpeed * front;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera->cameraPos -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera->cameraPos += glm::normalize(glm::cross(front, up)) * cameraSpeed;
	} else {
		front.y = 0.0f;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->cameraPos += cameraSpeed * front;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->cameraPos -= cameraSpeed * front;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera->cameraPos -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera->cameraPos += glm::normalize(glm::cross(front, up)) * cameraSpeed;
		//still want to be able to move the cam up and down
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera->cameraPos += cameraSpeed * up;
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			camera->cameraPos -= cameraSpeed * up;
	}
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//if the 1 key is pressed turn on/off wireframe mode
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		if (!wireframe_mode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireframe_mode = 1;
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireframe_mode = 0;
		}
	}
	//if escape key is pressed close window
	else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		if (!flip_mode) {
			flip_mode = 1;
		}
		else {
			flip_mode = 0;
		}
		respond_to_flip = 1;
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		if (!freefly_mode) {
			freefly_mode = 1;
		}
		else {
			freefly_mode = 0;
		}
	}
	else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		//Check that mix percentage is less than 1 and that it is not too close to 1.
		if (mix_percentage < 1.0f && !floatEquality(1.0f, mix_percentage)) {
			//Increase mix percentage 10% towards texture 2
			mix_percentage = mix_percentage + 0.1f;
			respond_to_mix_change = 1;
		}
		else {
			std::cout << "Mix Percentage Maxed!" << std::endl;
		}
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		if (mix_percentage > 0.0f && !floatEquality(0.0f, mix_percentage)) {
			//Increase mix percentage 10% towards texture 1
			mix_percentage = mix_percentage - 0.1f;
			respond_to_mix_change = 1;
		}
		else {
			std::cout << "Mix Percentage 0!" << std::endl;
		}
	}
	else if (key == GLFW_KEY_W && action == GLFW_PRESS) {

	}
}
bool firstMouse = true;
float lastx = WIDTH / 2.0f;
float lasty = HEIGHT / 2.0f;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	//define our starting mouse position
	Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstMouse) {
		lastx = xpos;
		lasty = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastx;
	float yoffset = lasty - ypos; //reversed since y-coordinates go from bottom to top
	lastx = xpos;
	lasty = ypos;

	xoffset *= camera->sensitivity;
	yoffset *= camera->sensitivity;

	camera->yaw += xoffset;
	camera->pitch += yoffset;
	//clamp pitch so that camera can look to the sky or to the ground but not flip upside down
	if (camera->pitch > 89.0f)
		camera->pitch = 89.0f;
	if (camera->pitch < -89.0f)
		camera->pitch = -89.0f;
	//no yaw clamp so the user can turn all the way around
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	camera->fov -= static_cast<float>(yoffset);
	if (camera->fov < 1.0f)
		camera->fov = 1.0f;
	if (camera->fov > 45.0f)
		camera->fov = 45.0f;
}
//Checks for relative equality between 2 floats.
bool floatEquality(float f1, float f2, float epsilon) {
	return std::fabs(f1 - f2) < epsilon;
}