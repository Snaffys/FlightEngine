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

    // Delete copy operations
    IBO(const IBO&) = delete;
    IBO& operator=(const IBO&) = delete;

    // Add move semantics
    IBO(IBO&& other) noexcept :
        id(other.id),
        indices(std::move(other.indices))
    {
        other.id = 0;
    }

    IBO& operator=(IBO&& other) noexcept {
        if (this != &other) {
            Delete();
            id = other.id;
            indices = std::move(other.indices);
            other.id = 0;
        }
        return *this;
    }

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