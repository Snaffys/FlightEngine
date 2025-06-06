//#ifndef TERRAIN_H
//#define TERRAIN_H
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//
//#include "Buffers/VAO.h"
//#include "Buffers/IBO.h"
//
//#include <thread>
//#include <mutex>
//
//#include "unordered_map"
//#include <queue>
//#include <functional>
//
//#include <random>
//#include <algorithm> 
//#include <limits>
//
//#include "Camera.h"
//
//#include "DB.h"
//
//namespace std {
//	template <>
//	struct hash<glm::vec2> {
//		size_t operator()(const glm::vec2& v) const {
//			// gets hash values of x and y
//			size_t h1 = hash<float>{}(v.x);
//			size_t h2 = hash<float>{}(v.y);
//			// Combine the two hashes
//			//return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
//			return h1 ^ (h2 << 1);
//		}
//	};
//}
//
//class Chunk;
//
//class LODChunk {
//public:
//	LODChunk() = default;
//	LODChunk(int lod);
//
//	void onChunkDataReceived(Chunk& chunk);
//
//	Chunk* getChunk();
//	bool getHasChunk();
//	bool getHasRequestedChunk();
//	int getLOD();
//	void setHasRequestedChunk(bool hasRequestedChunk);
//private:
//	int lod;
//	Chunk* Mesh;
//	bool hasRequestedMesh;
//	bool hasMesh;
//};
//
//struct LODInfo {
//	int lod;
//	float visibleDstThreshold;
//};
//
//class Chunk {
//public:
//	Chunk() = default;
//	Chunk(unsigned int vertexWidthAmount, unsigned int vertexHeightAmount, unsigned int octaveCount,
//		float scalingBias, glm::vec2 position, std::vector<LODInfo> detailLevels);
//
//	bool const operator==(const Chunk& other);
//
//	void generateChunkData(int lod);
//
//
//	void setLoaded(bool loaded);
//	void setNoiseSeed2D(float* noiseSeed2D);
//	void setPerlinNoise2D(float* perlinNoise2D);
//	void setVertices(std::vector<glm::vec3> vertices);
//	void setIndices(std::vector<unsigned int> indices);
//	TerrainVao getVAO();
//	IBO getIBO();
//	void setVBO(VBO vbo);
//	VBO getVBO();
//	void setVAO(TerrainVao vao);
//	void setIBO(IBO ibo);
//	int getPreviousLodIndex();
//	unsigned int getNumStrips();
//	unsigned int getNumVertsPerStrip();
//	void setPreviousLodIndex(int previousLodIndex);
//	void setNumStrips(unsigned int numStrips);
//	void setNumVertsPerStrip(unsigned int numVertsPerStrip);
//	LODChunk getLodChunk(int elementId);
//	LODChunk* getLodChunkPtr(int elementId);
//	std::vector<LODChunk> getLodChunks();
//	void setPosition(glm::vec2 position);
//
//	void setNormals(std::vector<glm::vec3> normals);
//
//
//	glm::vec2 getPosition();
//	unsigned int getVertexWidthAmount();
//	unsigned int getVertexHeightAmount();
//	unsigned int getOctaveCount();
//	float getScalingBias();
//	std::vector<glm::vec3> getVertices();
//	std::vector<glm::vec3>* getVerticesPtr();
//	std::vector<unsigned int> getIndices();
//	std::vector<unsigned int>* getIndicesPtr();
//	std::vector<glm::vec3> getNormals();
//	std::vector<glm::vec3>* getNormalsPtr();
//	float* getNoiseSeed2D();
//	float* getPerlinNoise2D();
//	bool getLoaded();
//
//
//private:
//	glm::vec2 position;
//
//	unsigned int vertexWidthAmount;
//	unsigned int vertexHeightAmount;
//	unsigned int octaveCount;
//	float scalingBias;
//
//	float* noiseSeed2D;	// random noise
//	float* perlinNoise2D;	// output perlin noise
//
//	std::vector<glm::vec3> vertices;
//	std::vector<unsigned int> indices;
//	std::vector<glm::vec3> normals;
//
//
//
//	bool loaded;
//
//	VBO vbo;
//	TerrainVao vao;
//	IBO ibo;
//
//	std::vector<LODChunk> lodChunks;
//
//	int previousLodIndex;
//
//	unsigned int numStrips;
//	unsigned int numVertsPerStrip;
//
//	void GenerateNoiseMap();
//
//	float lerp(float a, float b, float t);
//	float fade(float t);
//};
//
//struct ChunkThreadData {
//	std::function<void(Chunk&)> callback;
//	Chunk parameter;
//
//	ChunkThreadData(std::function<void(Chunk&)> callback, Chunk parameter)
//		: callback(callback), parameter(parameter) {}
//};
//
//class Terrain {
//public:
//	Terrain() = default;
//	Terrain(Camera* camera, unsigned int vertexWidthAmount = 257, unsigned int vertexHeightAmount = 257, unsigned int octaveCount = 8, float scalingBias = 0.5f);
//
//	Terrain(const Terrain& other);
//
//	Terrain& operator=(const Terrain& other);
//
//	void updateVisibleChunks();
//	void updateThread();
//
//	unsigned int getVertexWidthAmount();
//	unsigned int getVertexHeightAmount();
//	std::vector<Chunk*> getVisibleChunks();
//private:
//	int vertexWidthAmount;
//	int vertexHeightAmount;
//	unsigned int octaveCount;
//	float scalingBias;
//	float maxViewDst;
//	int chunkSize;
//	int chunksVisibleInViewDst;
//
//	Camera* camera;
//
//	std::unordered_map<glm::vec2, Chunk> chunks;
//	std::vector<Chunk*> visibleChunks;
//
//	std::queue<ChunkThreadData> mapDataThreadInfoQueue;
//	std::mutex queueMutex;
//
//	std::vector<LODInfo> detailLevels;
//
//	void chunkDataThreadStart(int lod, glm::vec2 pos, std::function<void(Chunk&)> callbackFunc);
//
//	void updateChunk(Chunk& chunk, int chunkCoordX, int chunkCoordY, float camPosX, float camPosZ);
//	void updateReceivedChunk(glm::vec2 chunkPos);
//
//	//DB db;
//};
//#endif











#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Buffers/VAO.h"
#include "Buffers/IBO.h"

#include <thread>
#include <mutex>

#include "unordered_map"
#include <queue>
#include <functional>

#include <random>
#include <algorithm> 
#include <limits>

#include "Camera.h"
#include "unordered_set"

#include "DB.h"

#include "ThreadPool.h"


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

	mutable bool needsGPUUpdate = false;


	bool hasMeshReady() const { return hasMesh; }

	// Add proper move semantics
	LODChunk(LODChunk&& other) noexcept
		: lod(other.lod),
		Mesh(std::move(other.Mesh)),
		hasRequestedMesh(other.hasRequestedMesh),
		hasMesh(other.hasMesh) {}

	LODChunk& operator=(LODChunk&& other) noexcept {
		lod = other.lod;
		Mesh = std::move(other.Mesh);
		hasRequestedMesh = other.hasRequestedMesh;
		hasMesh = other.hasMesh;
		return *this;
	}

	// Delete copy operations
	LODChunk(const LODChunk&) = delete;
	LODChunk& operator=(const LODChunk&) = delete;


	LODChunk(int lod);

	void onChunkDataReceived(Chunk& chunk);

	Chunk* getChunk() const;
	bool getHasChunk();
	bool getHasRequestedChunk();
	int getLOD();
	void setHasRequestedChunk(bool hasRequestedChunk);
	void setHasMesh(bool hasMesh);
private:
	int lod;
	//Chunk* Mesh;
	bool hasRequestedMesh;
	bool hasMesh;

	std::unique_ptr<Chunk> Mesh;
};

struct LODInfo {
	int lod;
	float visibleDstThreshold;
};

class Chunk {
public:
	size_t getCPUMemoryUsage() const {
		return vertices.capacity() * sizeof(glm::vec3) +
			normals.capacity() * sizeof(glm::vec3) +
			indices.capacity() * sizeof(unsigned int) +
			noiseSeed2D.capacity() * sizeof(float) +
			perlinNoise2D.capacity() * sizeof(float);
	};

	void logMemory(const std::string& context) const {
		std::cout << "Chunk " << position.x << "," << position.y
			<< " (" << context << "): "
			<< "Vertices=" << vertices.size() << "/" << vertices.capacity() << " "
			<< "Normals=" << normals.size() << "/" << normals.capacity() << " "
			<< "Indices=" << indices.size() << "/" << indices.capacity() << " "
			<< "NoiseSeed=" << noiseSeed2D.size() << "/" << noiseSeed2D.capacity() << " "
			<< "PerlinNoise=" << perlinNoise2D.size() << "/" << perlinNoise2D.capacity() << "\n";
	};
	unsigned int actualLOD = 0;

	~Chunk();
	struct LODData {
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;
		int lodLevel = -1;
		bool uploadedToGPU = false;
	};

	void cleanup();  // NEW


	void updateGPUResources();

	Chunk() = default;
	explicit Chunk(unsigned int vertexWidthAmount, unsigned int vertexHeightAmount, unsigned int octaveCount,
		float scalingBias, glm::vec2 position, std::vector<LODInfo> detailLevels);

	bool const operator==(const Chunk& other);

	Chunk& operator=(Chunk&& other) noexcept {
		if (this != &other) {
			// Move all members
			vertexWidthAmount = other.vertexWidthAmount;
			vertexHeightAmount = other.vertexHeightAmount;
			octaveCount = other.octaveCount;
			scalingBias = other.scalingBias;
			position = other.position;
			loaded = other.loaded;
			previousLodIndex = other.previousLodIndex;
			numStrips = other.numStrips;
			numVertsPerStrip = other.numVertsPerStrip;

			// Move containers
			noiseSeed2D = std::move(other.noiseSeed2D);
			perlinNoise2D = std::move(other.perlinNoise2D);
			vertices = std::move(other.vertices);
			indices = std::move(other.indices);
			normals = std::move(other.normals);

			// Move OpenGL resources
			vbo = std::move(other.vbo);
			vao = std::move(other.vao);
			ibo = std::move(other.ibo);

			// Move LOD chunks
			//lodChunks = std::move(other.lodChunks);
		}
		return *this;
	}

	Chunk(Chunk&& other) noexcept :
		vertexWidthAmount(other.vertexWidthAmount),
		vertexHeightAmount(other.vertexHeightAmount),
		octaveCount(other.octaveCount),
		scalingBias(other.scalingBias),
		position(other.position),
		loaded(other.loaded),
		previousLodIndex(other.previousLodIndex),
		numStrips(other.numStrips),
		numVertsPerStrip(other.numVertsPerStrip),
		noiseSeed2D(std::move(other.noiseSeed2D)),
		perlinNoise2D(std::move(other.perlinNoise2D)),
		vertices(std::move(other.vertices)),
		indices(std::move(other.indices)),
		normals(std::move(other.normals)),
		//lodChunks(std::move(other.lodChunks)),
		vbo(std::move(other.vbo)),
		vao(std::move(other.vao)),
		ibo(std::move(other.ibo))
	{
	}


	void generateChunkData(int lod);


	void setLoaded(bool loaded);
	void setNoiseSeed2D(const std::vector<float>& data);
	void setPerlinNoise2D(const std::vector<float>& data);
	void setVertices(std::vector<glm::vec3> vertices);
	void setIndices(std::vector<unsigned int> indices);

	void setIBO(std::unique_ptr<IBO> newIBO);
	IBO& getIBO();
	void setVAO(std::unique_ptr<TerrainVao> vao);
	TerrainVao& getVAO();
	void setVBO(std::unique_ptr<VBO> newVbo);
	VBO& getVBO();

	int getPreviousLodIndex();
	unsigned int getNumStrips();
	unsigned int getNumVertsPerStrip();
	void setPreviousLodIndex(int previousLodIndex);
	void setNumStrips(unsigned int numStrips);
	void setNumVertsPerStrip(unsigned int numVertsPerStrip);

	//LODChunk& getLodChunk(int elementId);
	//LODChunk* getLodChunkPtr(int elementId);
	//const std::vector<LODChunk>& getLodChunks();
	void setPosition(glm::vec2 position);

	void setNormals(std::vector<glm::vec3> normals);


	glm::vec2 getPosition();
	unsigned int getVertexWidthAmount() const;
	unsigned int getVertexHeightAmount() const;
	unsigned int getOctaveCount() const;
	float getScalingBias() const;
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec3>* getVerticesPtr();
	std::vector<unsigned int> getIndices();
	std::vector<unsigned int>* getIndicesPtr();
	std::vector<glm::vec3> getNormals();
	std::vector<glm::vec3>* getNormalsPtr();
	std::vector<float>& getNoiseSeed2D();
	std::vector<float>& getPerlinNoise2D();
	bool getLoaded() const;


	std::vector<glm::vec3>& getVerticesRef() { return vertices; }
	std::vector<glm::vec3>& getNormalsRef() { return normals; }
	std::vector<unsigned int>& getIndicesRef() { return indices; }



	std::vector<float> noiseSeed2D;
	std::vector<float> perlinNoise2D;

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;

	//std::vector<LODChunk> lodChunks;


	//std::unique_ptr<LODData> currentLod;  // NEW
	//bool lodDataGenerated = false;         // NEW

	int currentLOD = -1;
	bool needsUpdate = true;
	bool hasData = false;

	std::unique_ptr<VBO> vbo;
	std::unique_ptr<TerrainVao> vao;
	std::unique_ptr<IBO> ibo;

	void GenerateNoiseMap();

	//bool hasLODData() const { return currentLod != nullptr && lodDataGenerated; }
	//int getCurrentLodLevel() const { return currentLod ? currentLod->lodLevel : -1; }

private:
	glm::vec2 position;

	std::mutex resourceMutex;

	unsigned int vertexWidthAmount;
	unsigned int vertexHeightAmount;
	unsigned int octaveCount;
	float scalingBias;

	bool loaded;

	int previousLodIndex;

	unsigned int numStrips;
	unsigned int numVertsPerStrip;


	float lerp(float a, float b, float t);
	float fade(float t);
};


struct ChunkThreadData {
    std::function<void(Chunk&)> callback;
    glm::vec2 chunkPos;  // Store position instead of Chunk

    ChunkThreadData(std::function<void(Chunk&)> callback, glm::vec2 pos)
        : callback(callback), chunkPos(pos) {}
};

class Terrain {
public:

	size_t getTotalCPUMemoryUsage() const {
		size_t total = 0;
		for (const auto& [pos, chunk] : chunks) {
			total += chunk.getCPUMemoryUsage();
		}
		return total;
	};

	Terrain() = default;
	Terrain(Camera* camera, DB* db, unsigned int vertexWidthAmount = 257, unsigned int vertexHeightAmount = 257, unsigned int octaveCount = 8, float scalingBias = 0.5f);

	Terrain(const Terrain& other);

	Terrain& operator=(const Terrain& other);

	void updateVisibleChunks();
	void updateThread();

	unsigned int getVertexWidthAmount();
	unsigned int getVertexHeightAmount();
	std::unordered_set<Chunk*> getVisibleChunks();

	ThreadPool pool{ std::thread::hardware_concurrency() };
	//ThreadPool pool(4);

	std::unordered_map<glm::vec2, Chunk> chunks;

private:
	DB* db;
	int vertexWidthAmount;
	int vertexHeightAmount;
	unsigned int octaveCount;
	float scalingBias;
	float maxViewDst;
	int chunkSize;
	int chunksVisibleInViewDst;

	Camera* camera;


	std::unordered_set<Chunk*> visibleChunks;

	std::queue<std::pair<std::function<void(Chunk&)>, glm::vec2>> mapDataThreadInfoQueue;
	std::mutex queueMutex;

	std::vector<LODInfo> detailLevels;

	void chunkDataThreadStart(int lod, glm::vec2 pos, std::function<void(Chunk&)> callbackFunc);

	void updateChunk(Chunk& chunk, int chunkCoordX, int chunkCoordY, float camPosX, float camPosZ);
	void updateReceivedChunk(glm::vec2 chunkPos);



	std::unordered_set<glm::vec2> processingChunks;
	std::mutex processingMutex;


	//DB db;
};
#endif















































































//
//#ifndef TERRAIN_H
//#define TERRAIN_H
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//
//#include "Buffers/VAO.h"
//#include "Buffers/IBO.h"
//
//#include <thread>
//#include <mutex>
//
//#include "unordered_map"
//#include <queue>
//#include <functional>
//
//#include <random>
//#include <algorithm> 
//#include <limits>
//
//#include "Camera.h"
//
//#include "DB.h"
//
//namespace std {
//	template <>
//	struct hash<glm::vec2> {
//		size_t operator()(const glm::vec2& v) const {
//			// gets hash values of x and y
//			size_t h1 = hash<float>{}(v.x);
//			size_t h2 = hash<float>{}(v.y);
//			// Combine the two hashes
//			//return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
//			return h1 ^ (h2 << 1);
//		}
//	};
//}
//
//class Chunk;
//
//class LODChunk {
//public:
//	LODChunk() = default;
//	LODChunk(int lod);
//
//	void onChunkDataReceived(Chunk& chunk);
//
//	Chunk* getChunk();
//	bool getHasChunk();
//	bool getHasRequestedChunk();
//	int getLOD();
//	void setHasRequestedChunk(bool hasRequestedChunk);
//private:
//	int lod;
//	Chunk* Mesh;
//	bool hasRequestedMesh;
//	bool hasMesh;
//};
//
//struct LODInfo {
//	int lod;
//	float visibleDstThreshold;
//};
//
//class Chunk {
//public:
//	Chunk() = default;
//	Chunk(unsigned int vertexWidthAmount, unsigned int vertexHeightAmount, unsigned int octaveCount,
//		float scalingBias, glm::vec2 position, std::vector<LODInfo> detailLevels);
//
//	bool const operator==(const Chunk& other);
//
//	void generateChunkData(int lod);
//
//
//	void setLoaded(bool loaded);
//	void setNoiseSeed2D(float* noiseSeed2D);
//	void setPerlinNoise2D(float* perlinNoise2D);
//	void setVertices(std::vector<glm::vec3> vertices);
//	void setIndices(std::vector<unsigned int> indices);
//	TerrainVao getVAO();
//	IBO getIBO();
//	void setVBO(VBO vbo);
//	VBO getVBO();
//	void setVAO(TerrainVao vao);
//	void setIBO(IBO ibo);
//	int getPreviousLodIndex();
//	unsigned int getNumStrips();
//	unsigned int getNumVertsPerStrip();
//	void setPreviousLodIndex(int previousLodIndex);
//	void setNumStrips(unsigned int numStrips);
//	void setNumVertsPerStrip(unsigned int numVertsPerStrip);
//	LODChunk getLodChunk(int elementId);
//	LODChunk* getLodChunkPtr(int elementId);
//	std::vector<LODChunk> getLodChunks();
//	void setPosition(glm::vec2 position);
//
//	void setNormals(std::vector<glm::vec3> normals);
//
//
//	glm::vec2 getPosition();
//	unsigned int getVertexWidthAmount();
//	unsigned int getVertexHeightAmount();
//	unsigned int getOctaveCount();
//	float getScalingBias();
//	std::vector<glm::vec3> getVertices();
//	std::vector<glm::vec3>* getVerticesPtr();
//	std::vector<unsigned int> getIndices();
//	std::vector<unsigned int>* getIndicesPtr();
//	std::vector<glm::vec3> getNormals();
//	std::vector<glm::vec3>* getNormalsPtr();
//	float* getNoiseSeed2D();
//	float* getPerlinNoise2D();
//	bool getLoaded();
//
//
//private:
//	glm::vec2 position;
//
//	unsigned int vertexWidthAmount;
//	unsigned int vertexHeightAmount;
//	unsigned int octaveCount;
//	float scalingBias;
//
//	float* noiseSeed2D;	// random noise
//	float* perlinNoise2D;	// output perlin noise
//
//	std::vector<glm::vec3> vertices;
//	std::vector<unsigned int> indices;
//	std::vector<glm::vec3> normals;
//
//
//
//	bool loaded;
//
//	VBO vbo;
//	TerrainVao vao;
//	IBO ibo;
//
//	std::vector<LODChunk> lodChunks;
//
//	int previousLodIndex;
//
//	unsigned int numStrips;
//	unsigned int numVertsPerStrip;
//
//	void GenerateNoiseMap();
//
//	float lerp(float a, float b, float t);
//	float fade(float t);
//};
//
//struct ChunkThreadData {
//	std::function<void(Chunk&)> callback;
//	Chunk parameter;
//
//	ChunkThreadData(std::function<void(Chunk&)> callback, Chunk parameter)
//		: callback(callback), parameter(parameter) {}
//};
//
//class Terrain {
//public:
//	Terrain() = default;
//	Terrain(Camera* camera, unsigned int vertexWidthAmount = 257, unsigned int vertexHeightAmount = 257, unsigned int octaveCount = 8, float scalingBias = 0.5f);
//
//	Terrain(const Terrain& other);
//
//	Terrain& operator=(const Terrain& other);
//
//	void updateVisibleChunks();
//	void updateThread();
//
//	unsigned int getVertexWidthAmount();
//	unsigned int getVertexHeightAmount();
//	std::vector<Chunk*> getVisibleChunks();
//
//
//	std::unordered_map<glm::vec2, Chunk> chunks;
//
//private:
//	int vertexWidthAmount;
//	int vertexHeightAmount;
//	unsigned int octaveCount;
//	float scalingBias;
//	float maxViewDst;
//	int chunkSize;
//	int chunksVisibleInViewDst;
//
//	Camera* camera;
//
//	std::vector<Chunk*> visibleChunks;
//
//	std::queue<ChunkThreadData> mapDataThreadInfoQueue;
//	std::mutex queueMutex;
//
//	std::vector<LODInfo> detailLevels;
//
//	void chunkDataThreadStart(int lod, glm::vec2 pos, std::function<void(Chunk&)> callbackFunc);
//
//	void updateChunk(Chunk& chunk, int chunkCoordX, int chunkCoordY, float camPosX, float camPosZ);
//	void updateReceivedChunk(glm::vec2 chunkPos);
//
//	//DB db;
//};
//#endif



















//#ifndef TERRAIN_H
//#define TERRAIN_H
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//
//#include "Buffers/VAO.h"
//#include "Buffers/IBO.h"
//
//#include <thread>
//#include <mutex>
//
//#include "unordered_map"
//#include <queue>
//#include <functional>
//
//#include <random>
//#include <algorithm> 
//#include <limits>
//#include <unordered_set>
//
//#include "Camera.h"
//
//#include "DB.h"
//
//struct ChunkKey {
//	glm::vec2 position;
//	int lod;  // Level of detail
//
//	bool operator==(const ChunkKey& other) const {
//		return position == other.position && lod == other.lod;
//	}
//};
//
//namespace std {
//	template <>
//	struct hash<glm::vec2> {
//		size_t operator()(const glm::vec2& v) const {
//			// gets hash values of x and y
//			size_t h1 = hash<float>{}(v.x);
//			size_t h2 = hash<float>{}(v.y);
//			// Combine the two hashes
//			//return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
//			return h1 ^ (h2 << 1);
//		}
//	};
//}
//
//class Chunk;
//
//class LODChunk {
//public:
//	LODChunk() = default;
//	LODChunk(int lod);
//
//	void onChunkDataReceived(Chunk& chunk);
//
//	Chunk* getChunk();
//	bool getHasChunk();
//	bool getHasRequestedChunk();
//	int getLOD();
//	void setHasRequestedChunk(bool hasRequestedChunk);
//private:
//	int lod;
//	Chunk* Mesh;
//	bool hasRequestedMesh;
//	bool hasMesh;
//};
//
//struct LODInfo {
//	int lod;
//	float visibleDstThreshold;
//};
//
//class Chunk {
//public:
//	Chunk() = default;
//	Chunk(unsigned int vertexWidthAmount, unsigned int vertexHeightAmount, unsigned int octaveCount,
//		float scalingBias, glm::vec2 position, std::vector<LODInfo> detailLevels);
//
//
//	bool const operator==(const Chunk& other);
//
//	void generateChunkData(int lod);
//
//
//	void setLoaded(bool loaded);
//
//	void setVertices(std::vector<glm::vec3> vertices);
//	void setIndices(std::vector<unsigned int> indices);
//	TerrainVao getVAO();
//	IBO getIBO();
//	void setVBO(VBO vbo);
//	VBO getVBO();
//	void setVAO(TerrainVao vao);
//	void setIBO(IBO ibo);
//	int getPreviousLodIndex();
//	unsigned int getNumStrips();
//	unsigned int getNumVertsPerStrip();
//	void setPreviousLodIndex(int previousLodIndex);
//	void setNumStrips(unsigned int numStrips);
//	void setNumVertsPerStrip(unsigned int numVertsPerStrip);
//	LODChunk getLodChunk(int elementId);
//	LODChunk* getLodChunkPtr(int elementId);
//	std::vector<LODChunk> getLodChunks();
//	void setPosition(glm::vec2 position);
//
//	void setNormals(std::vector<glm::vec3> normals);
//
//
//	glm::vec2 getPosition();
//	unsigned int getVertexWidthAmount();
//	unsigned int getVertexHeightAmount();
//	unsigned int getOctaveCount();
//	float getScalingBias();
//	std::vector<glm::vec3> getVertices();
//	std::vector<glm::vec3>* getVerticesPtr();
//	std::vector<unsigned int> getIndices();
//	std::vector<unsigned int>* getIndicesPtr();
//	std::vector<glm::vec3> getNormals();
//	std::vector<glm::vec3>* getNormalsPtr();
//	std::vector<float>& getNoiseSeed2D();
//	std::vector<float>& getPerlinNoise2D();
//	bool getLoaded();
//	void GenerateNoiseMap();
//
//	void setPerlinNoise2D(const std::vector<float>& data);
//	void setNoiseSeed2D(const std::vector<float>& data);
//
//
//
//	std::vector<float> noiseSeed2D;
//	std::vector<float> perlinNoise2D;
//
//	std::vector<glm::vec3> vertices;
//	std::vector<unsigned int> indices;
//	std::vector<glm::vec3> normals;
//	std::vector<LODChunk> lodChunks;
//
//private:
//	glm::vec2 position;
//
//	unsigned int vertexWidthAmount;
//	unsigned int vertexHeightAmount;
//	unsigned int octaveCount;
//	float scalingBias;
//
//	//float* noiseSeed2D;	// random noise
//	//float* perlinNoise2D;	// output perlin noise
//
//
//
//
//
//	bool loaded;
//
//	VBO vbo;
//	TerrainVao vao;
//	IBO ibo;
//
//
//
//	int previousLodIndex;
//
//	unsigned int numStrips;
//	unsigned int numVertsPerStrip;
//
//
//
//	float lerp(float a, float b, float t);
//	float fade(float t);
//};
//
//struct ChunkThreadData {
//	std::function<void(Chunk&)> callback;
//	Chunk parameter;
//
//	ChunkThreadData(std::function<void(Chunk&)> callback, Chunk parameter)
//		: callback(callback), parameter(parameter) {}
//};
//
//class Terrain {
//public:
//	Terrain() = default;
//	Terrain(Camera* camera, unsigned int vertexWidthAmount = 257, unsigned int vertexHeightAmount = 257, unsigned int octaveCount = 8, float scalingBias = 0.5f);
//	int getLODForDistance(float distance);
//
//	Terrain(const Terrain& other);
//
//	Terrain& operator=(const Terrain& other);
//
//	void updateVisibleChunks();
//	void updateThread();
//
//	unsigned int getVertexWidthAmount();
//	unsigned int getVertexHeightAmount();
//	std::vector<Chunk*> getVisibleChunks();
//private:
//	int vertexWidthAmount;
//	int vertexHeightAmount;
//	unsigned int octaveCount;
//	float scalingBias;
//	float maxViewDst;
//	int chunkSize;
//	int chunksVisibleInViewDst;
//
//	Camera* camera;
//
//	std::unordered_map<glm::vec2, Chunk> chunks;
//	std::vector<Chunk*> visibleChunks;
//
//	std::queue<ChunkThreadData> mapDataThreadInfoQueue;
//	std::mutex queueMutex;
//
//	std::vector<LODInfo> detailLevels;
//
//	void chunkDataThreadStart(int lod, glm::vec2 pos, std::function<void(Chunk&)> callbackFunc);
//
//	void updateChunk(Chunk& chunk, int chunkCoordX, int chunkCoordY, float camPosX, float camPosZ);
//	void updateReceivedChunk(glm::vec2 chunkPos);
//
//	//DB db;
//};
//#endif
