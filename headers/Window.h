#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <stb/stb_image.h>

//#include "Shader.h"
//#include "VAO.h"
//#include "Camera.h"
//#include "IBO.h"
#include "FBO.h"
//#include "Texture.h"
#include "Terrain.h"
#include "Model.h"

#include "unordered_map"
//#include <glm/vec2.hpp>
#include <functional>
#include <thread>

#include <queue>

//#include <thread>
//#include <mutex>
//#include <queue>

class Window {
public:
	Window(unsigned int width = 1024, unsigned int height = 1024);

	void launch();

	~Window();
private:

	void ProcessMouseMove1(float x_offset, float y_offset);


	GLFWwindow* window;
	static Window* windowInstance;
	int width, height;
	Camera camera;

	double lastX, lastY;
	bool firstMouse = true;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;




	// input callbacks
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void mouseCallback(GLFWwindow* window, double xPos, double yPos);
	void processInput(GLFWwindow* window);

	Shader terrainShader;
	Shader postProcShader;
	Shader cubemapShader;
	Shader planeShader;
	Shader terrainShaderNormals;
	Shader modelShader;

	Texture cubemapTex;
	Texture postProcTex;

	VBO terrainVbo;
	VBO postProcVbo;
	VBO cubemapVbo;
	VBO planeVbo;
	TerrainVao terrainVao;
	Pos3DVao planeVao;
	VAO postProcVao;
	VAO cubemapVao;
	IBO terrainIbo;

	FBO postProcFbo;

	float exposure = 1.0f;

	Terrain terrain;
	int nOutputDepth = 257;
	int nOctaveCount = 8;
	float fScalingBias = 2.0f;
	unsigned int levelOfDetail = 0;
	bool polygonMode = false;
	bool octavePressed = false;

	const float viewerMoveThreasholdForChnkUpdt = 25;
	const float sqrViewerMoveThresholdForChunkUpdate = viewerMoveThreasholdForChnkUpdt * viewerMoveThreasholdForChnkUpdt;
	glm::vec2 cameraPostionOld = glm::vec2(0,0);



	float planeRotationDegreesHorizontal = 0;
	float planeRotationDegreesVertical = 0;
	glm::vec3 planePosition = glm::vec3(0.0);
	glm::vec3 planeFront = glm::vec3(0, 0, -1);


	bool freeCamera = true;
	bool shiftWasPressed = false;
	bool freeCamPrevious = true;
	glm::vec2 savedCursorPos = glm::vec2(0,0);
	float turnRate = 0;
	glm::quat cameraOrientation = glm::quat(1, 0, 0, 0);

	Model modelM;
};



#endif
