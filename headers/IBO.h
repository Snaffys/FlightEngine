#ifndef IBO_H
#define IBO_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <initializer_list>

#include <glm/glm.hpp>

struct Index {
	Index() = default;
	Index(unsigned int first, unsigned int second, unsigned int third) : indices(first, second, third) {};
	glm::uvec3 indices;
};

class IBO {
public:
	IBO() = default;
	IBO(std::initializer_list<Index> index);
	IBO(std::vector<unsigned int> index);

	int getId();
	void Bind();
	void Unbind();
	void Delete();

	~IBO();
private:
	std::vector<Index> indices;
	GLuint id;
};

#endif