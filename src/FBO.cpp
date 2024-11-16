#include <FBO.h>

FBO::FBO(unsigned int width, unsigned int height, Texture& texture, GLenum format, GLenum internalFormat) {
	glGenFramebuffers(1, &framebufferId);
	BindFramebuffer();

	texture = { format, width, height, GL_FLOAT, GL_COLOR_ATTACHMENT0, internalFormat };

	glGenRenderbuffers(1, &renderbufferId);
	BindRenderbuffer();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferId);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("ERROR::FRAMEBUFFER::Framebuffer is not complete!");
		exit(1);
	}

	UnbindFramebuffer();
}

void FBO::BindFramebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
}
void FBO::UnbindFramebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FBO::DeleteFramebuffer()
{
	glDeleteFramebuffers(1, &framebufferId);
}

void FBO::BindRenderbuffer() {
	glBindRenderbuffer(GL_RENDERBUFFER, renderbufferId);
}
void FBO::UnbindRenderbuffer() {
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
void FBO::DeleteRenderbuffer()
{
	glDeleteRenderbuffers(1, &renderbufferId);
}