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

#include "Model/tiny_gltf.h"

#include "filesystem"



class Texture {
public:
	Texture() = default;

	Texture(const char* texturePath);

	Texture(const tinygltf::Image& image, const tinygltf::Model& model, const char* filePath);

	Texture(GLenum format, const unsigned int imgWidth, const unsigned int imgHeight, GLenum type, GLenum textarget, GLenum internalFormat);

	//Texture(const char* image, const char* texType, GLuint slot);

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

	std::vector<unsigned char> base64_decode(const std::string& encoded);
	int imgWidth, imgHeight, numColCh;
	unsigned char* data = nullptr;
	std::string type;
	std::string filename;
protected:
	GLuint id = NULL;
};

#endif