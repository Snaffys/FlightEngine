#ifndef VBO_H
#define VBO_H

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>
#include <initializer_list>

struct PosNormTex {
	PosNormTex() = default;

	PosNormTex(float x, float y, float z, float n_x, float n_y, float n_z, float s, float t)
		: pos(x, y, z), normals(n_x, n_y, n_z), texCoords(s, t) {};

	glm::vec3 pos;
	glm::vec3 normals;
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

	VBO(std::initializer_list<PosNormTex> coords);
	VBO(std::initializer_list<PosTex2D> coords);
	VBO(std::initializer_list<Pos3D> coords);
	VBO(std::vector<float> vertices);

	int getId() {
		return id;
	}

    void Bind();
	void Unbind();
	void Delete();

	~VBO();
private:

	std::vector<PosNormTex> posNormTexVec;
	std::vector<PosTex2D> posTex2DVec;
	std::vector<Pos3D> pos3DVec;

    unsigned int id;
};

#endif