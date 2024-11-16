#include <IBO.h>

IBO::IBO(std::initializer_list<Index> index) : indices(index) {
	// Generates buffer
	glGenBuffers(1, &id);
	// Binds buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	// Creates and initialize EBO data store
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Index), &indices[0], GL_STATIC_DRAW);
}

IBO::IBO(std::vector<unsigned int> index) {
	// Generates buffer
	glGenBuffers(1, &id);
	// Binds buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	// Creates and initialize EBO data store
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(index[0]), &index[0], GL_STATIC_DRAW);
}

// Binds buffer
void IBO::Bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

// Unbinds buffer
void IBO::Unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Deletes buffer
void IBO::Delete()
{
	Unbind();
	glDeleteBuffers(1, &id);
}

IBO::~IBO() {
	//Delete();
}