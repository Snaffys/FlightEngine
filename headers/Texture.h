#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <stdio.h>

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



class Texture {
public:
	Texture() = default;

	Texture(GLenum format, const unsigned int imgWidth, const unsigned int imgHeight, GLenum type, GLenum textarget, GLenum internalFormat);

	Texture(const int rows, const int cols, const char* faces);

	void Bind();
	void Unbind();
	void Delete();

	void SetId(GLuint id);
	GLuint GetId();
	void SetType(std::string type);
	std::string GetType();
	void SetFilename(std::string filename);
	std::string GetFilename();
private:
	int imgWidth, imgHeight, numColChan;
	unsigned char* data;
	std::string type;
	std::string filename;
protected:
	GLuint id = NULL;
};

#endif