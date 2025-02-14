#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "VAO.h"
#include "IBO.h"

#include <thread>
#include <mutex>

#include "unordered_map"
#include <queue>
#include <functional>

#include <random>
#include <algorithm> 
#include <limits>

#include "Camera.h"

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

class Chunk;

class LODChunk {
public:
	LODChunk() = default;
	LODChunk(int lod);

	void onChunkDataReceived(Chunk& chunk);

	Chunk* getChunk();
	bool getHasChunk();
	bool getHasRequestedChunk();
	int getLOD();
	void setHasRequestedChunk(bool hasRequestedChunk);
private:
	int lod;
	Chunk* Mesh;
	bool hasRequestedMesh;
	bool hasMesh;
};

struct LODInfo {
	int lod;
	float visibleDstThreshold;
};

class Chunk {
public:
	Chunk() = default;
	Chunk(unsigned int vertexWidthAmount, unsigned int vertexHeightAmount, unsigned int octaveCount,
		float scalingBias, float maxViewDst, glm::vec2 position, std::vector<LODInfo> detailLevels);
	
	bool const operator==(const Chunk& other);

	void generateChunkData(int lod);

	glm::vec2 getPosition();
	bool getLoaded();
	void setLoaded(bool loaded);
	void setNoiseSeed2D(float* noiseSeed2D);
	float* getNoiseSeed2D();
	void setPerlinNoise2D(float* perlinNoise2D);
	float* getPerlinNoise2D();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec3>* getVerticesPtr();
	void setVertices(std::vector<glm::vec3> vertices);
	std::vector<unsigned int> getIndices();
	std::vector<unsigned int>* getIndicesPtr();
	void setIndices(std::vector<unsigned int> indices);
	TerrainVao getVAO();
	IBO getIBO();
	void setVBO(VBO vbo);
	VBO getVBO();
	void setVAO(TerrainVao vao);
	void setIBO(IBO ibo);
	int getPreviousLodIndex();
	unsigned int getNumStrips();
	unsigned int getNumVertsPerStrip();
	void setPreviousLodIndex(int previousLodIndex);
	void setNumStrips(unsigned int numStrips);
	void setNumVertsPerStrip(unsigned int numVertsPerStrip);
	LODChunk getLodChunk(int elementId);
	LODChunk* getLodChunkPtr(int elementId);
	std::vector<LODChunk> getLodChunks();
	void setPosition(glm::vec2 position);
	std::vector<glm::vec3> getNormals();
	std::vector<glm::vec3>* getNormalsPtr();
	void setNormals(std::vector<glm::vec3> normals);

private:
	int vertexWidthAmount;
	int vertexHeightAmount;
	unsigned int octaveCount;
	float scalingBias;
	float maxViewDst;

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;
	glm::vec2 position;

	float* noiseSeed2D;	// random noise
	float* perlinNoise2D;	// output perlin noise

	bool loaded;

	VBO vbo;
	TerrainVao vao;
	IBO ibo;

	std::vector<LODChunk> lodChunks;

	int previousLodIndex;

	unsigned int numStrips;
	unsigned int numVertsPerStrip;

	void createPerlinNoise2D();

	float lerp(float a, float b, float t);
	float fade(float t);
};

struct ChunkThreadData {
	std::function<void(Chunk&)> callback;
	Chunk parameter;

	ChunkThreadData(std::function<void(Chunk&)> callback, Chunk parameter)
		: callback(callback), parameter(parameter) {}
};

class Terrain {
public:
	Terrain() = default;
	Terrain(Camera* camera, unsigned int vertexWidthAmount = 257, unsigned int vertexHeightAmount = 257, unsigned int octaveCount = 8, float scalingBias = 2.0f);

	Terrain(const Terrain& other);

	Terrain& operator=(const Terrain& other);

	void updateVisibleChunks();
	void updateThread();

	unsigned int getVertexWidthAmount();
	unsigned int getVertexHeightAmount();
	std::vector<Chunk*> getVisibleChunks();
private:
	int vertexWidthAmount;
	int vertexHeightAmount;
	unsigned int octaveCount;
	float scalingBias;
	float maxViewDst;
	int chunkSize;
	int chunksVisibleInViewDst;

	Camera* camera;

	std::unordered_map<glm::vec2, Chunk> chunks;
	std::vector<Chunk*> visibleChunks;

	std::queue<ChunkThreadData> mapDataThreadInfoQueue;
	std::mutex queueMutex;

	std::vector<LODInfo> detailLevels;

	void chunkDataThreadStart(int lod, glm::vec2 pos, std::function<void(Chunk&)> callbackFunc);

	void updateChunk(Chunk& chunk, int chunkCoordX, int chunkCoordY, float camPosX, float camPosZ);
	void updateReceivedChunk(glm::vec2 chunkPos);
};
#endif