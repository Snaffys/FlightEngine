#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <stb/stb_image.h>

#include "Shader.h"
#include "VAO.h"
#include "Camera.h"
#include "IBO.h"
#include "FBO.h"
#include "Texture.h"


struct Chunk {
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	glm::vec2 position;

	bool loaded;

	VBO terrainVboC;
	TerrainVao terrainVaoC;
	IBO terrainIboC;
};


class Window {
public:
	Window(unsigned int width = 1024, unsigned int height = 1024);

	void launch();

	~Window();
private:
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void mouseCallback(GLFWwindow* window, double xPos, double yPos);
	void processInput(GLFWwindow* window);

	void UpdateVisibleChunks();

	void generateTerrain(Chunk& chunk);

	float lerp(float a, float b, float t);
	float fade(float t);

	GLFWwindow* window;
	int width, height;

	Shader terrainShader;
	Shader postProcShader;
	Shader cubemapShader;
	Shader planeShader;

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

	Camera camera;
	double lastX, lastY;
	bool firstMouse = true;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	float exposure = 1.0f;

	static Window* windowInstance;

	bool polygonMode = false;

	int nOctaveCount = 8;


	float fScalingBias = 2.0f;

	bool octavePressed = false;

	// should be odd for proper LOD calculation
	int nOutputWidth = 257;
	int nOutputHeight = 257;
	int nOutputDepth = 257;

	float* fNoiseSeed2D = nullptr;	// random noise
	float* fPerlinNoise2D = nullptr;	// output perlin noise

	void PerlinNoise2D(int nWidth, int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutput);
	void PerlinNoise3D(int mapWidth, int mapHeight, int mapDepth, float* fSeed, int nOctaves, float bias, float* mapOutput);


	std::vector<Chunk> chunks;

	std::vector<Chunk> lastVisibleChunks;

	void updateChunk(Chunk& chunk, unsigned int maxViewDst, int chunkCoordX, int chunkCoordY);

	unsigned int levelOfDetail = 0;

	float maxViewDst = 600;
	int chunkSize = nOutputWidth - 1;
	int chunksVisibleInViewDst = int(maxViewDst) / chunkSize;

	void test();

	void generateTerrain3D(Chunk& chunk);
};

#endif
