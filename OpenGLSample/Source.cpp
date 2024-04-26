//JOSE D GONZALEZ
//CS-330
//Module 7 Project
//------------------------------------------------------------------


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "shader.h"
// adding myMesh h file
#include "myMesh.h"
// adding my shapes h file
#include "shapes.h"
// including the cylinder h file and shape generators
#include "cylinder.h"




//Resize Window--------------------------------------------------------------------------
// fucntion to rezise viewport if window gets resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//Perspective Variable----------------------------------------------------------------------------
// viewTypePerspective bool variable
bool viewTypePerspective = false;

//TextureLoad Function------------------------------------------------------------------------
unsigned int loadTexture(const char* path);

//Mouse Functions-----------------------------------------------------------------------------------
// when the mouse moves this function is called 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// scroll wheel input calls this function
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Key Input Function------------------------------------------------------------------------------
// function to read input from user if they pressed the escape key
void processInput(GLFWwindow* window);

//Timing------------------------------------------------------------------------------
// delta and last frame times global variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Camera------------------------------------------------------------------------------
// camera settings with pos, front, and up vecs
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//Mouse-------------------------------------------------------------------------------
bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

//Screen Settings------------------------------------------------------------------------------
const unsigned int height = 1200;
const unsigned int width = 1400;

// Lights---------------------------------------------------------------------------------------
// positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(-5.0f, -4.0f,  negativeZ),
	glm::vec3(-5.0f, -4.0f, middleZ),
	glm::vec3(-5.0f, -4.0f, positveZ),
	glm::vec3(5.0f, -4.0f, negativeZ),
	glm::vec3(5.0f,  -4.0f, middleZ),
	glm::vec3(5.0f,  -4.0f, positveZ),
};


//MAIN------------------------------------------------------------------------------
int main() {
	// initializing GLFW with version 4 of OpenGL
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creating Window------------------------------------------------------------------------------------
	// using glfwCreateWindow to create a window object with 800,600 size and my name
	GLFWwindow* window = glfwCreateWindow(width, height, "Jose D Gonzalez -- Game of Thrones: Throne Room", NULL, NULL);
	if (window == NULL) {
		std::cout << "FAILED TO CREATE GLFW WINDOW." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// telling GLFW to call function on every window resize
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// telling GLFW to call mouse function
	glfwSetCursorPosCallback(window, mouse_callback);
	// telling GLFW to call scroll function
	glfwSetScrollCallback(window, scroll_callback);
	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Initializing Glad-------------------------------------------------------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "FAILED TO INITIALIZE GLAD" << std::endl;
		return -1;
	}

	//Shaders---------------------------------------------------------------------------------------
	// utilizing the shader class using shader.h and shader.cpp 
	// to compile and build the shaders
	// I made my own shader txt files in resources files so I can practice building my own shaders from scratch
	Shader myShader("Vert.vs", "Frag.fs");

	//Z-Buffer---------------------------------------------------------------------------------------------
	glEnable(GL_DEPTH_TEST);

	//Texture------------------------------------------------------------------------------
	
	// calling the loadTexure function 
	unsigned int floor = loadTexture("tiles.jpg");
	unsigned int pillarBase = loadTexture("texture.jpg");
	unsigned int pillar = loadTexture("marble.jpg");
	unsigned int wall = loadTexture("wall.jpg");
	unsigned int wall2 = loadTexture("wall_diffuse");
	unsigned int light = loadTexture("fire.jpg");
	unsigned int throne = loadTexture("throne.jpg");
	unsigned int gold = loadTexture("gold.jpg");
	unsigned int steps = loadTexture("stone.jpg");
	unsigned int banner = loadTexture("got.jpg");
	stbi_set_flip_vertically_on_load(true);

	// using texture vectors to fully utilize mesh
	vector<Texture> planeTexture = {
			{floor},
	};
	vector<Texture> cubeTexture = {
			{pillarBase},
	};
	vector<Texture> wallTexture = {
			{wall},
			{wall2},
	};
	vector<Texture> throneTexture = {
			{throne},
	};
	vector<Texture> lightTexture = {
			{light},
	};
	vector<Texture> pyramidTexture = {
			{gold},
	};
	vector<Texture> stepsTexture = {
			{steps},
	};
	vector<Texture> bannerTexture = {
			{banner},
	};



	//Mesh------------------------------------------------------------------------------------------
	// using myMesh to create a mesh
	Mesh cubeMesh(cubeVertices, cubeIndices, cubeTexture);
	Mesh planeMesh(planeVertices, planeIndices, planeTexture);
	Mesh wallMesh(wallVertices, wallIndices, wallTexture);
	Mesh throneMesh(throneVertices, throneIndices, throneTexture);
	Mesh pyramidMesh(pyramidVertices, pyramidIndices, pyramidTexture);
	Mesh stepMesh(stepVertices, stepIndices, stepsTexture);
	Mesh bannerMesh(bannerVertices, bannerIndices, bannerTexture);
	Mesh swordMesh(swordVertices, swordIndices, throneTexture);

	//Light------------------------------------------------------------------------------------------
	Mesh lightMesh(lightCubeVertices, lightCubeIndices, lightTexture);
	
	
	//Model Variables-----------------------------------------------------------------------------------------------
	glm::mat4 model;
	float angle;
	
	// ---------------------------------------------------------------------------------------------
	// render loop
	while (!glfwWindowShouldClose(window)) {
		// frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// calling the processInput function to see if escape was pressed
		processInput(window);

		// rendering commands
		// clearing screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // screen set to black
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// activating shader program
		myShader.use();
		myShader.setVec3("viewPos", cameraPosition);
		myShader.setFloat("material.shininess", 50.0f);
		//Lights---------------------------------------------------------------------------------------
		// directional light
	/*	myShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		myShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		myShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		myShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);*/
		//light 1
		myShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		myShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		myShader.setVec3("pointLights[0].diffuse", 0.68f, 0.32f, 0.0f);
		myShader.setVec3("pointLights[0].specular", 0.68f, 0.32f, 0.0f);
		myShader.setFloat("pointLights[0].constant", 0.0f);
		myShader.setFloat("pointLights[0].linear", 0.09f);
		myShader.setFloat("pointLights[0].quadratic", 0.032f);
		//light 2
		myShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		myShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		myShader.setVec3("pointLights[1].diffuse", 0.68f, 0.32f, 0.0f);
		myShader.setVec3("pointLights[1].specular", 0.68f, 0.32f, 0.0f);
		myShader.setFloat("pointLights[1].constant", 0.0f);
		myShader.setFloat("pointLights[1].linear", 0.09f);
		myShader.setFloat("pointLights[1].quadratic", 0.032f);
		//light 3
		myShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		myShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		myShader.setVec3("pointLights[2].diffuse", 0.68f, 0.32f, 0.0f);
		myShader.setVec3("pointLights[2].specular", 0.68f, 0.32f, 0.0f);
		myShader.setFloat("pointLights[2].constant", 0.0f);
		myShader.setFloat("pointLights[2].linear", 0.09f);
		myShader.setFloat("pointLights[2].quadratic", 0.032f);
		//light 4
		myShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		myShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		myShader.setVec3("pointLights[3].diffuse", 0.68f, 0.32f, 0.0f);
		myShader.setVec3("pointLights[3].specular", 0.68f, 0.32f, 0.0f);
		myShader.setFloat("pointLights[3].constant", 0.0f);
		myShader.setFloat("pointLights[3].linear", 0.09f);
		myShader.setFloat("pointLights[3].quadratic", 0.032f);
		//light 5
		myShader.setVec3("pointLights[4].position", pointLightPositions[4]);
		myShader.setVec3("pointLights[4].ambient", 0.05f, 0.05f, 0.05f);
		myShader.setVec3("pointLights[4].diffuse", 0.68f, 0.32f, 0.0f);
		myShader.setVec3("pointLights[4].specular", 0.68f, 0.32f, 0.0f);
		myShader.setFloat("pointLights[4].constant", 0.0f);
		myShader.setFloat("pointLights[4].linear", 0.09f);
		myShader.setFloat("pointLights[0].quadratic", 0.032f);
		//light 6
		myShader.setVec3("pointLights[5].position", pointLightPositions[5]);
		myShader.setVec3("pointLights[5].ambient", 0.05f, 0.05f, 0.05f);
		myShader.setVec3("pointLights[5].diffuse", 0.68f, 0.32f, 0.0f);
		myShader.setVec3("pointLights[5].specular", 0.68f, 0.32f, 0.0f);
		myShader.setFloat("pointLights[5].constant", 0.0f);
		myShader.setFloat("pointLights[5].linear", 0.09f);
		myShader.setFloat("pointLights[5].quadratic", 0.032f);
		// spotLight
		myShader.setVec3("spotLight.position", glm::vec3(0.0f, 6.0f, -34.0f));
		myShader.setVec3("spotLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));
		myShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		myShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		myShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		myShader.setFloat("spotLight.constant", 0.0f);
		myShader.setFloat("spotLight.linear", 0.09f);
		myShader.setFloat("spotLight.quadratic", 0.032f);
		myShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		myShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		
		//initilazing view, and projection spaces with identity matrix
		glm::mat4 view = glm::mat4(1.0f); // identity matrix first
		glm::mat4 projection = glm::mat4(1.0f); // identity matrix first
		
		// utilizing the lookAt function to set up camera position with my camera settings
		view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
		myShader.setMat4("view", view);
		// changing projections depending on user input of P
		if (!viewTypePerspective) {
			projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);
		}
		else {
			projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
		}
		myShader.setMat4("projection", projection);

		// plane-------------------------------------------------------------------------------------------
		model = glm::mat4(1.0f); // identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -15.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));
		myShader.setMat4("model", model);
		// rendering plane
		planeMesh.Draw(myShader);

		// throne-------------------------------------------------------------------------------------------
		model = glm::mat4(1.0f); // identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, -2.9f, -34.0f));
		model = glm::scale(model, glm::vec3(1.3f, 1.3f, 1.3f));
		myShader.setMat4("model", model);
		// rendering throne
		throneMesh.Draw(myShader);


		// wallPlane-------------------------------------------------------------------------------------------
		for (int i = 0; i < 3; i++) {
			model = glm::mat4(1.0f); // identity matrix first		
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -15.0f));
			model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));
			model = glm::rotate(model, glm::radians(wallAngles[i]), glm::vec3(0.0f, 1.0f, 0.0f));
			myShader.setMat4("model", model);
			// rendering plane
			wallMesh.Draw(myShader);
		};


		// pillarBase cube---------------------------------------------------------------------------------------------
		for (int i = 0; i < 6; i++) {
			model = glm::mat4(1.0f); // identity matrix first		
			model = glm::translate(model, basePositions[i]);
			model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));
			angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
			myShader.setMat4("model", model);
			// rendering cube
			cubeMesh.Draw(myShader);
		};
		// pillar cylinder----------------------------------------------------------------------------------------------
		for (int i = 0; i < 6; i++) {
			// activating and binding texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, pillar);
			model = glm::mat4(1.0f); // identity matrix first	
			model = glm::translate(model, pillarPositions[i]);
			model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));
			myShader.setMat4("model", model);

			static_meshes_3D::Cylinder C(0.7, 20, 17, true, true, false);
			C.render();
		};

		// pyramid-----------------------------------------------------------------------------------------------
		for (int i = 0; i < 6; i++) {
			model = glm::mat4(1.0f); // identity matrix first		
			model = glm::translate(model, brazierPositions[i]);
			model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));
			angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
			myShader.setMat4("model", model);
			// rendering cube
			pyramidMesh.Draw(myShader);
		};

		// steps-------------------------------------------------------------------------------------------
		for (int i = 0; i < 4; i++) {
			model = glm::mat4(1.0f); // identity matrix first		
			model = glm::translate(model, stepPositions[i]);
			model = glm::scale(model, stepSizes[i]);
			myShader.setMat4("model", model);
			// rendering planes
			stepMesh.Draw(myShader);
		};

		// banner-------------------------------------------------------------------------------------------
		model = glm::mat4(1.0f); // identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, 5.0, -37.2f));
		model = glm::scale(model, glm::vec3(6.0f, 6.0f, 1.0f));
		myShader.setMat4("model", model);
		// rendering banner
		bannerMesh.Draw(myShader);

		//swords-----------------------------------------------------------------------------------------------
		for (int i = 0; i < 6; i++) {
			model = glm::mat4(1.0f); // identity matrix first		
			model = glm::translate(model, swordPositions[i]);
			model = glm::scale(model, glm::vec3(0.4f, swordSizes[i], 1.0f));
			//angle = 45.0f;
			//model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
			myShader.setMat4("model", model);
			// rendering sword
			swordMesh.Draw(myShader);
		}

		// lightCube objects--------------------------------------------------------------------------------------
		for (int i = 0; i < 6; i++) {
			model = glm::mat4(1.0f); // identity matrix first
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.19f, 0.19f, 0.19f));
			myShader.setMat4("model", model);
			// rendering cube
			lightMesh.Draw(myShader);
		}
		
		// checks for triggered events & swapping buffers so image gets rendered each frame
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	// deleting window
	glfwDestroyWindow(window);
	// clean/delete of GLFW resources
	glfwTerminate();
	return 0;
}

// when the mouse moves this function is called 
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.2f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}
// scroll wheel input calls this function
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// fov being updated by the mouse scroll wheel 
	// scroll up for faster and scroll down for slower
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}


// ------------------------------------------------------------------------------
// function to read input from user if they pressed the escape key
void processInput(GLFWwindow* window) {
	// if condition to check to see if the input was the ecape key
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		// setting the window should close to true
		glfwSetWindowShouldClose(window, true);
	// using WSADQE key input to move around with a constant frame speed
	float cameraSpeed = static_cast<float>(fov * deltaTime); // multipying deltaTime by fov which depends of the mouse scroll 
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPosition += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPosition -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPosition += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPosition -= cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) // P input to change perspective
		if (!viewTypePerspective) {
			viewTypePerspective = true;
		}
		else {
			viewTypePerspective = false;
		}

};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// initializing viewport with w and h passed in the function
	glViewport(0, 0, width, height);
};

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	stbi_set_flip_vertically_on_load(true);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
};