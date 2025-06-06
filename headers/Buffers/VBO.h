#ifndef VBO_H
#define VBO_H

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>
#include <initializer_list>

#include "ParticleManager/Particle.h"

struct PosNormTex {
	PosNormTex() = default;

	PosNormTex(float x, float y, float z, float n_x, float n_y, float n_z, float s, float t)
		: pos(x, y, z), normals(n_x, n_y, n_z), texCoords(s, t) {};

	PosNormTex(glm::vec3 pos, glm::vec3 normals, glm::vec2 texCoords)
		: pos(pos), normals(normals), texCoords(texCoords) {};

	glm::vec3 pos;
	glm::vec3 normals;
	glm::vec2 texCoords;
};

struct PosNormTexCol {
	PosNormTexCol() = default;

	PosNormTexCol(float x, float y, float z, float n_x, float n_y, float n_z, float s, float t, float r, float g, float b)
		: pos(x, y, z), normals(n_x, n_y, n_z), texCoords(s, t), color(r, g, b) {};

	PosNormTexCol(glm::vec3 pos, glm::vec3 normals, glm::vec2 texCoords, glm::vec3 color)
		: pos(pos), normals(normals), texCoords(texCoords), color(color) {};

	glm::vec3 pos;
	glm::vec3 normals;
	glm::vec2 texCoords;
	glm::vec3 color;
};

struct PosTex3D {
	PosTex3D() = default;

	PosTex3D(float x, float y, float z, float s, float t) : pos(x, y, z), texCoords(s, t) {};

	glm::vec3 pos;
	glm::vec2 texCoords;
};

struct PosTex2D {
	PosTex2D() = default;

	PosTex2D(float x, float y, float s, float t) : pos(x, y), texCoords(s, t) {};

	glm::vec2 pos;
	glm::vec2 texCoords;
};


struct Pos3D {
	Pos3D() = default;

	Pos3D(float x, float y, float z) : pos(x, y, z) {};

	glm::vec3 pos;
};

class VBO {
public:
    VBO() = default;

	// Delete copy operations
	VBO(const VBO&) = delete;
	VBO& operator=(const VBO&) = delete;

	// Add move semantics
	VBO(VBO&& other) noexcept :
		id(other.id),
		vertexCount(other.vertexCount),
		POSITIONS(other.POSITIONS),
		posNormTexVec(std::move(other.posNormTexVec)),
		posNormTexColVec(std::move(other.posNormTexColVec)),
		posTex2DVec(std::move(other.posTex2DVec)),
		pos3DVec(std::move(other.pos3DVec))
	{
		other.id = 0; // Important: prevent double deletion
	}

	VBO& operator=(VBO&& other) noexcept {
		if (this != &other) {
			Delete(); // Clean up existing resources

			// Transfer ownership
			id = other.id;
			vertexCount = other.vertexCount;
			POSITIONS = other.POSITIONS;
			posNormTexVec = std::move(other.posNormTexVec);
			posNormTexColVec = std::move(other.posNormTexColVec);
			posTex2DVec = std::move(other.posTex2DVec);
			pos3DVec = std::move(other.pos3DVec);

			other.id = 0; // Leave source in valid state
		}
		return *this;
	}

	VBO(std::initializer_list<PosNormTex> coords);
	VBO(std::initializer_list<PosNormTexCol> coords);
	VBO(std::vector<PosNormTexCol> coords);
	VBO(std::vector<PosNormTex>& coords);
	VBO(std::initializer_list<PosTex2D> coords);
	VBO(std::initializer_list<PosTex3D> coords);
	VBO(std::initializer_list<Pos3D> coords);
	VBO(std::vector<float> vertices);
	VBO(std::vector<glm::vec3> vecVertices);

	VBO(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals);
	VBO(std::vector<Particle>& instances);

	int getId() {
		return id;
	}

	std::size_t POSITIONS;

    void Bind() const;
	void Unbind() const;
	void Delete();
	unsigned int getVertexCount();

	~VBO();
private:
	

	std::vector<PosNormTex> posNormTexVec;
	std::vector<PosNormTexCol> posNormTexColVec;
	std::vector<PosTex2D> posTex2DVec;
	std::vector<PosTex3D> posTex3DVec;
	std::vector<Pos3D> pos3DVec;

    unsigned int id;
	unsigned int vertexCount;
};

#endif