#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "VBO.h"
#include "IBO.h"


class VAO {
public:
    VAO() = default;
	VAO(VBO& vbo);

	void MatInstance(const int index, VBO& VBO);
	void VecInstance(int index, VBO& VBO);

	void Bind();
	void Unbind();
	void Delete();

	~VAO();
protected:
	GLuint id;
};

class PosTex2DVao : public VAO {
public:
	PosTex2DVao() = default;
	PosTex2DVao(VBO& VBO);
};

class Pos3DVao : public VAO {
public:
	Pos3DVao() = default;
	Pos3DVao(VBO& VBO);
};

class TerrainVao : public VAO {
public:
	TerrainVao() = default;
	TerrainVao(VBO& VBO);
};

#endif