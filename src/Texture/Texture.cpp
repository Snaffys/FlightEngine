#include <Texture/Texture.h>

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

//Texture::Texture(const char* image, const char* texType, GLuint slot){
//	type = texType;
//	int imagWidth, imgHeight, numColCh;
//	stbi_set_flip_vertically_on_load(true);
//	unsigned char* bytes = stbi_load(image, &imgWidth, &imgHeight, &numColCh, 0);
//	if (!bytes)
//		throw std::invalid_argument("ERROR: Failed to load texture\n");
//	glGenTextures(1, &id);
//	glActiveTexture(GL_TEXTURE + slot);
//	//unit = slot;
//	Bind();
//
//
//
//	if (numColCh == 4)
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
//	else if (numColCh == 3)
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
//	else if (numColCh == 1)
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, imgWidth, imgHeight, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
//	else
//		throw std::invalid_argument("Texture type error\n");
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	glGenerateMipmap(GL_TEXTURE_2D);
//	stbi_image_free(bytes);
//	Unbind();
//}

Texture::Texture(const char* texPath) {
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	glBindTexture(GL_TEXTURE_2D, id);
	stbi_set_flip_vertically_on_load(false);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;

	data = stbi_load(texPath, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0,
			GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	stbi_set_flip_vertically_on_load(true);
}


Texture::Texture(const tinygltf::Image& image, const tinygltf::Model& model, const char* filePath) {
	stbi_set_flip_vertically_on_load(false);
	if (!image.uri.empty() && image.uri.find("data:") == std::string::npos) {
		std::string fullPath = std::filesystem::path(filePath).parent_path().string() + "/" + image.uri;
		//std::string fullPath = "res/models/bunny/" + image.uri;
		data = stbi_load(fullPath.c_str(), &imgWidth, &imgHeight, &numColCh, 0);
		if (!data) {
			std::cerr << "Failed to load texture from file: " << fullPath << std::endl;
			std::cerr << "STB Image error: " << stbi_failure_reason() << std::endl;
			return;
		}
	}
	else if (!image.uri.empty() && image.uri.find("data:") != std::string::npos) {
		std::string base64Data = image.uri.substr(image.uri.find(",") + 1);
		std::vector<unsigned char> decodedData = base64_decode(base64Data);
		data = stbi_load_from_memory(decodedData.data(), decodedData.size(), &imgWidth, &imgHeight, &numColCh, 0);
		if (!data) {
			std::cerr << "Failed to load texture from data URI" << std::endl;
			std::cerr << "STB Image error: " << stbi_failure_reason() << std::endl;
			return;
		}
	}
	else if (image.bufferView >= 0) {
		const tinygltf::BufferView& bufferView = model.bufferViews[image.bufferView];
		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
		data = stbi_load_from_memory(&buffer.data[bufferView.byteOffset], bufferView.byteLength, &imgWidth, &imgHeight, &numColCh, 0);
		if (!data) {
			std::cerr << "Failed to load texture from buffer view" << std::endl;
			std::cerr << "STB Image error: " << stbi_failure_reason() << std::endl;
			return ;
		}
	}
	else {
		std::cerr << "Unsupported image format" << std::endl;
		return;
	}
	
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	Bind();

	if (numColCh == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else if (numColCh == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else if (numColCh == 1)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, imgWidth, imgHeight, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	else
		throw std::invalid_argument("Texture type error\n");

	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}



std::vector<unsigned char> Texture::base64_decode(const std::string& encoded) {
	const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	std::vector<unsigned char> decoded;
	int i = 0;
	uint32_t val = 0;
	int bits = 0;

	for (char c : encoded) {
		if (c == '=') break;

		size_t pos = base64_chars.find(c);
		if (pos == std::string::npos) continue;

		val = (val << 6) | pos;
		bits += 6;

		if (bits >= 8) {
			bits -= 8;
			decoded.push_back((val >> bits) & 0xFF);
		}
	}

	return decoded;
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