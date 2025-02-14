#include <Texture.h>

Texture::Texture(GLenum format, const unsigned int img_width, const unsigned int img_height, GLenum type, GLenum textarget, GLenum internal_format) {
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, img_width, img_height, 0, format, type, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, textarget, GL_TEXTURE_2D, id, 0);
}

Texture::Texture(const char* image, const char* texType, GLuint slot){
	type = texType;
	int imagWidth, imgHeight, numColCh;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image, &imgWidth, &imgHeight, &numColCh, 0);
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE + slot);
	//unit = slot;
	Bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (numColCh == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	else if (numColCh == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	if (numColCh == 1)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
	else
		throw std::invalid_argument("Texture type error\n");

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(bytes);
	Unbind();
}

Texture::Texture(const int rows, const int cols, const char* faces) {
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	stbi_set_flip_vertically_on_load(false);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < rows; i++)
	{
		data = stbi_load((char*)(faces + i * (cols)), &width, &height, &nrChannels, 0);
		if (data)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
				GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else
			printf("Cubemap texture failed to load at path: %s\n", ((char*)(faces + i * (cols))));
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	stbi_set_flip_vertically_on_load(true);
}


void Texture::Bind() {
	glBindTexture(GL_TEXTURE_2D, id);
}
void Texture::Unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::Delete()
{
	glDeleteTextures(1, &id);
}

GLuint Texture::GetId() {
	return id - 1;
}

std::string Texture::GetType() {
	return type;
}