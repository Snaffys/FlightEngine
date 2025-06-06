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
#include "Buffers/FBO.h"
//#include "Texture.h"
#include "Terrain/Terrain.h"
#include "Model/Model.h"

#include "unordered_map"
//#include <glm/vec2.hpp>
#include <functional>
#include <thread>

#include <queue>

#include "DB.h"
#include "Aircraft.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Shader.h"
#include "ParticleManager/ParticleGenerator.h"


//#include <thread>
//#include <mutex>
//#include <queue>

class Window {
public:
	Window(unsigned int width = 1024, unsigned int height = 1024);

	void launch();

	~Window();
private:
	std::vector<Particle>  particles;
	unsigned int nrParticles = 500;


	void processCameraRotation(float turnRate, float pitch, float roll);


	GLFWwindow* window;
	static Window* windowInstance;
	int width, height;
	Camera camera;

	double lastX, lastY;
	bool firstMouse = true;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;


	float currentRoll = 0.0f;

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

	std::unique_ptr<Terrain> terrain;
	//Terrain terrain;
	int nOutputDepth = 257;
	int nOctaveCount = 8;
	float fScalingBias = 2.0f;
	unsigned int levelOfDetail = 0;
	bool polygonMode = false;
	bool octavePressed = false;

	const float viewerMoveThreasholdForChnkUpdt = 25;
	const float sqrViewerMoveThresholdForChunkUpdate = viewerMoveThreasholdForChnkUpdt * viewerMoveThreasholdForChnkUpdt;
	glm::vec2 cameraPostionOld = glm::vec2(0,0);




	glm::vec3 planePosition = glm::vec3(0.0);
	glm::vec3 planeFront = glm::vec3(0, 0, -1);


	bool freeCamera = true;
	bool shiftWasPressed = false;
	bool freeCamPrevious = true;
	double savedCursorPosX = 0;
	double savedCursorPosY = 0;
	float turnRate = 0;
	glm::quat cameraOrientation = glm::quat(1, 0, 0, 0);

	Model planeModel;

	glm::vec3 planeUp = glm::vec3(0.0f, 1.0f, 0.0f);
	bool firstHalf = true;

	//DB db;
	Aircraft aircraft;


	glm::quat planeOrientation = glm::quat(1, 0, 0, 0);


	glm::vec3 planeFor = glm::vec3(0.0f,0.0f,-1.0f);

	glm::vec3 planeRight = glm::vec3(1.0f,0.0f,0.0f);
	void rollChange(float roll);
	void pitchChange(float pitch);
	//void yawChange(float yaw);
	void yawChange();

	float smoothstep(float edge0, float edge1, float x);
	bool resetRollAngle = false;
	bool resetPitchAngle = false;

	void resetRoll();

	DB db;

	ParticleGenerator snowParticles;
	ParticleGenerator fireParticles;
};



#endif
