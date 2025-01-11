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

#include "unordered_map"

#include <functional>
#include <thread>

#include <queue>


#include <thread>
#include <mutex>
#include <queue>
#include <functional> // For std::function


#include <future>  // For std::async

struct Chunk {
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	glm::vec2 position;

	bool loaded;

	VBO terrainVboC;
	TerrainVao terrainVaoC;
	IBO terrainIboC;
};

namespace std {
	template <>
	struct hash<glm::vec2> {
		size_t operator()(const glm::vec2& v) const {
			// gets hash values of x and y
			size_t h1 = hash<float>{}(v.x);
			size_t h2 = hash<float>{}(v.y);
			// Combine the two hashes
			//return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
			return h1 ^ (h2 << 1);
		}
	};
}

struct MapThreadInfo {
	// Define the callback type as a function taking T and returning void
	void (*callback)(Chunk&);
	Chunk parameter;

	// Constructor to initialize callback and parameter
	MapThreadInfo(void (*callback)(Chunk&), Chunk parameter)
		: callback(callback), parameter(parameter) {}
};


class Window {
public:
	Window(unsigned int width = 1024, unsigned int height = 1024);

	void launch();

	~Window();
private:
	bool hasRequestedMesh;
	bool hasMesh;
	int lod;


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
	int vertexWidthAmount = 257;
	int vertexHeightAmount = 257;
	int nOutputDepth = 257;

	float* fNoiseSeed2D = nullptr;	// random noise
	float* fPerlinNoise2D = nullptr;	// output perlin noise


	float* fNoiseSeed3D = nullptr;	// random noise
	float* fPerlinNoise3D = nullptr;	// output perlin noise

	void PerlinNoise2D(int nWidth, int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutput);
	void PerlinNoise3D(int mapWidth, int mapHeight, int mapDepth, float* fSeed, int nOctaves, float bias, float* mapOutput);

	float worleyNoise3D(int x, int y, int z);
	int hash3D(int x, int y, int z);


	//std::vector<Chunk> chunks;
	std::unordered_map<glm::vec2, Chunk> chunks;

	std::vector<Chunk> lastVisibleChunks;

	void updateChunk(Chunk& chunk, unsigned int maxViewDst, int chunkCoordX, int chunkCoordY);

	unsigned int levelOfDetail = 0;

	float maxViewDst = 600;
	int chunkSize = vertexWidthAmount - 1;
	int chunksVisibleInViewDst = int(maxViewDst) / chunkSize;

	std::mutex queueMutex;
	std::queue<MapThreadInfo> mapDataThreadInfoQueue;
	void requestMapData(glm::vec2 chunkPos, void (*callbackFunc)(Chunk&));
	void mapDataThread(glm::vec2 chunkPos, void (*callbackFunc)(Chunk&));
	void updateThread();
	static void onMapDataReceived(Chunk& chunk);

	void LODMesh(int lod);
	void RequestMesh(Chunk chunk);

};

#endif
