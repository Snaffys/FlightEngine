#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Buffers/VBO.h"
#include "Buffers/IBO.h"


class VAO {
public:
    VAO() = default;
	VAO(VBO& vbo);

	bool IsValid();

	void MatInstance(const int index, VBO& VBO);
	void VecInstance(int index, VBO& VBO);

	void Bind();
	void Unbind();
	void Delete();


	VAO(const VAO&) = delete;
	VAO& operator=(const VAO&) = delete;

	// Enable move operations
	VAO(VAO&& other) noexcept : id(other.id) { other.id = 0; }
	VAO& operator=(VAO&& other) noexcept {
		if (this != &other) {
			Delete();
			id = other.id;
			other.id = 0;
		}
		return *this;
	}

	~VAO();
protected:
	GLuint id;
};

class PosTex2DVao : public VAO {
public:
	PosTex2DVao() = default;
	PosTex2DVao(VBO& VBO);
};

class PosTex3DVao : public VAO {
public:
	PosTex3DVao() = default;
	PosTex3DVao(VBO& VBO);
};

class Pos3DVao : public VAO {
public:
	Pos3DVao() = default;
	Pos3DVao(VBO& VBO);
};

class TerrainVao : public VAO {
public:
	TerrainVao() = default;
	TerrainVao(const VBO& VBO);
	//TerrainVao(VBO VBO);
};

class PosNormTexColVao : public VAO {
public:
	PosNormTexColVao() = default;
	PosNormTexColVao(VBO& VBO);
};

class ParticleVAO : public VAO {
public:
	ParticleVAO() = default;
	ParticleVAO(VBO& quadVBO, VBO& instanceVBO);
};

#endif