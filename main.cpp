#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>
#include <fstream>

#include "Shader.h"
#include "Camera.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 760

bool press = false;
double mouseX = 0.0;
double mouseY = 0.0;

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), 5.0f);

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		switch (action)
		{
			case GLFW_PRESS:
				press = true;
				glfwGetCursorPos(window, &mouseX, &mouseY);
				break;

			case GLFW_RELEASE:
				press = false;
				break;
		}
	}
}

void cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
	if (press)
	{
		camera.orbit(xpos - mouseX, ypos - mouseY);

		mouseX = xpos;
		mouseY = ypos;
	}
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	camera.zoom(-yoffset);
}

void traverse(const aiScene *scene, aiNode* node, std::vector<GLfloat> &vertices, std::vector<GLuint> &indices)
{
	for (unsigned i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

		for (unsigned j = 0; j < mesh->mNumVertices; ++j)
		{
			vertices.push_back(mesh->mVertices[j].x);
			vertices.push_back(mesh->mVertices[j].y);
			vertices.push_back(mesh->mVertices[j].z);
			/*
			   vertices.emplace_back((GLfloat) mesh->mVertices[j].x);
			   vertices.emplace_back((GLfloat) mesh->mVertices[j].y);
			   vertices.emplace_back((GLfloat) mesh->mVertices[j].z);
			   */
		}

		for (unsigned j = 0; j < mesh->mNumFaces; ++j)
		{
			// Triangle
			indices.emplace_back(mesh->mFaces[j].mIndices[0]);
			indices.emplace_back(mesh->mFaces[j].mIndices[1]);
			indices.emplace_back(mesh->mFaces[j].mIndices[2]);
		}
	}

	for (unsigned i = 0; i < node->mNumChildren; ++i)
		traverse(scene, node->mChildren[i], vertices, indices);
}

void loadModel(const char *filename, std::vector<GLfloat> &vertices, std::vector<GLuint> &indices)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "error: " << importer.GetErrorString() << std::endl;
		return;
	}

	traverse(scene, scene->mRootNode, vertices, indices);
}

void setupModel(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices, GLuint *VAO, GLuint *VBO, GLuint *EBO)
{
	// VAO
	glGenVertexArrays(1, VAO);
	glBindVertexArray(*VAO);

	// VBO
	glGenBuffers(1, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
	glEnableVertexAttribArray(0);

	// EBO
	glGenBuffers(1, EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void deleteModel(GLuint *VAO, GLuint *VBO, GLuint *EBO)
{
	glDeleteVertexArrays(1, VAO);
	glDeleteBuffers(1, VBO);
	glDeleteBuffers(1, EBO);
}

using namespace std;
int main(int argc,char *argv[])
{
	//readfile
	fstream input;
	input.open(argv[1], ios::binary|ios::in);
	char buffer[16];
	uint16_t rainRow, rainCol, maxHeight, rainFrameNumber;
	input.read(reinterpret_cast<char*>(&rainRow),sizeof(rainRow));
	//rainRow = 5;
	input.read(reinterpret_cast<char*>(&rainCol),sizeof(rainCol));
	input.read(reinterpret_cast<char*>(&maxHeight),sizeof(maxHeight));
	maxHeight *= 0.1;
	input.read(reinterpret_cast<char*>(&rainFrameNumber),sizeof(rainFrameNumber));
	cout << rainRow<<" ,"<<rainCol<<", "<<maxHeight<<endl;
	uint16_t rainSpeed, rainDuration;
	uint16_t rain[rainRow][rainCol];
	input.read(reinterpret_cast<char*>(&rainSpeed),sizeof(rainSpeed));
	input.read(reinterpret_cast<char*>(&rainDuration),sizeof(rainDuration));
	for(unsigned int i=0;i<rainRow;i++){
		for(unsigned int j=0;j<rainCol;j++){
			input.read(reinterpret_cast<char*>(&rain[i][j]),sizeof(uint16_t));
			if((i%2)==0){
				rain[i][j]*=-0.5;
				rain[i][j]+=150;
			}
		}
	}

	// GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	//For MacOS
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "abc", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "glfwCreateWindow fail" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "glewInit fail" << std::endl;
		return -1;
	}

	// GL
	//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	// Shaders
	Shader cuboidShader("cuboidVertexShader.c", "cuboidFragmentShader.c");
	Shader sphereShader("sphereVertexShader.c", "sphereFragmentShader.c");

	// Cuboid vertices
	float cubeHigh = maxHeight*0.3;
	float cubeRow = rainRow*0.3;
	float cubeCol = rainCol*0.3;
	float cuboidVertices[] = {
		// Position         // Color
		-0.15f, cubeHigh, -0.15f, 
		cubeRow, cubeHigh, -0.15f,  
		-0.15f, cubeHigh, cubeCol,  
		cubeRow, cubeHigh, cubeCol,   

		-0.15f, -0.15f, -0.15f, 
		cubeRow, -0.15f, -0.15f,  
		-0.15f, -0.15f, cubeCol,  
		cubeRow, -0.15f, cubeCol,   
	};

	// Cuboid indices
	GLuint cuboidIndices[] = {
		0, 1,
		1, 3,
		3, 2,
		2, 0,
		4, 5,
		5, 7,
		7, 6,
		6, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	// Cuboid VAO
	GLuint cuboidVAO;
	glGenVertexArrays(1, &cuboidVAO);
	glBindVertexArray(cuboidVAO);

	// Cuboid VBO
	GLuint cuboidVBO;
	glGenBuffers(1, &cuboidVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cuboidVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cuboidVertices), cuboidVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);
	glEnableVertexAttribArray(0);

	// Cuboid EBO
	GLuint cuboidEBO;
	glGenBuffers(1, &cuboidEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cuboidEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cuboidIndices), cuboidIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Sphere
	std::vector<GLfloat> sphereVertices;
	std::vector<GLuint> sphereIndices;
	loadModel("sphere.obj", sphereVertices, sphereIndices);// filename argv[0]
	GLuint sphereVAO, sphereVBO, sphereEBO;
	setupModel(sphereVertices, sphereIndices, &sphereVAO, &sphereVBO, &sphereEBO);

	// Sphere heights
	GLuint heights[] = {
		0, 0, 0,
		0, 0, 0
	};

	float speed = 10;
	double currentTime, preTime=0, deltaTime;
	double position[rainRow][rainCol];
	for(unsigned int i=0;i < rainRow;i++){
		for(unsigned int j=0;j<rainCol;j++){
			position[i][j]=0;
		}
	}

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view(camera.getViewMatrix());
		glm::mat4 projection(glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 1000.0f));
		//glm::mat4 projection(glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 1000.0f));

		glUseProgram(cuboidShader.getProgram());
		GLint modelLocation = glGetUniformLocation(cuboidShader.getProgram(), "model");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
		GLint viewLocation = glGetUniformLocation(cuboidShader.getProgram(), "view");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
		GLint projectionLocation = glGetUniformLocation(cuboidShader.getProgram(), "projection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(cuboidVAO);
		glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		currentTime = glfwGetTime();

		glm::vec4 colorEach(1.0f, 1.0f, 0.0f, 1.0f);
		deltaTime = currentTime -preTime;
		for (unsigned i = 0; i < rainRow; i++)
		{
			for (unsigned j = 0; j < rainCol; j++)
			{
				// model = translate * scale
				if((i%2)==0)
					colorEach=glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
				glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.1*position[i][j], j) * glm::vec3(0.3));
				model = glm::scale(model, glm::vec3(0.1f));

				glUseProgram(sphereShader.getProgram());
				modelLocation = glGetUniformLocation(sphereShader.getProgram(), "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				viewLocation = glGetUniformLocation(sphereShader.getProgram(), "view");
				glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
				projectionLocation = glGetUniformLocation(sphereShader.getProgram(), "projection");
				glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
				GLint  colorLocation= glGetUniformLocation(sphereShader.getProgram(), "colorEach");
				glUniform4fv(colorLocation, 1, glm::value_ptr(colorEach));

				glBindVertexArray(sphereVAO);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}
		int count=0;
		for(unsigned int i=0;i < rainRow;i++){
			for(unsigned int j=0;j<rainCol;j++){
				if(position[i][j]<rain[i][j]){
					position[i][j]+=deltaTime*speed;
					if(position[i][j]>=rain[i][j])
						position[i][j]=rain[i][j];
				}
				else if(position[i][j]>rain[i][j]){
					position[i][j]-=deltaTime*speed;
					if(position[i][j]<=rain[i][j])
						position[i][j]=rain[i][j];
				}
				if(position[i][j]==rain[i][j])
					count++;
			}
		}
		if(count==rainRow*rainCol){
			input.read(reinterpret_cast<char*>(&rainSpeed),sizeof(rainSpeed));
			input.read(reinterpret_cast<char*>(&rainDuration),sizeof(rainDuration));
			for(unsigned int i=0;i<rainRow;i++){
				for(unsigned int j=0;j<rainCol;j++){
					input.read(reinterpret_cast<char*>(&rain[i][j]),sizeof(uint16_t));
				}
			}
		}
		preTime = currentTime;
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &cuboidVAO);
	glDeleteBuffers(1, &cuboidVBO);
	glDeleteBuffers(1, &cuboidEBO);
	deleteModel(&sphereVAO, &sphereVBO, &sphereEBO);

	glfwTerminate();
}
