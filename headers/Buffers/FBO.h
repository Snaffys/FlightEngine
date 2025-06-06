#ifndef FBO_H
#define FBO_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Texture/Texture.h"

class FBO {
public:
	FBO() = default;
	FBO(unsigned int width, unsigned int height, Texture& texture, GLenum format = GL_RGBA, GLenum internalFormat = GL_RGBA16F);

	void BindFramebuffer();
	void UnbindFramebuffer();
	void DeleteFramebuffer();

	void BindRenderbuffer();
	void UnbindRenderbuffer();
	void DeleteRenderbuffer();
private:
	GLuint renderbufferId = NULL;
protected:
	GLuint framebufferId = NULL;
};

#endif