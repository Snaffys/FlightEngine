#ifndef MESH_H
#define MESH_H

#include"VBO.h"
#include"VAO.h"
#include"IBO.h"
#include"Camera.h"
#include"Texture.h"
#include"Shader.h"

class Mesh {
public:
	std::vector<PosNormTex> vertexData;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	VAO vao;
	VBO vbo;
	IBO ibo;

	Mesh(std::vector<PosNormTex>& vertexData, std::vector<unsigned int>& indices, std::vector<Texture>& textures);
	void Draw(Shader& shader, Camera& camera, glm::mat4 matrix = glm::mat4(1.0f), glm::vec3 translation = glm::vec3(0.0f), 
		glm::quat rotation = glm::quat(1.0f,0.0f,0.0f,0.0f), glm::vec3 scale = glm::vec3(1.0f));
};

#endif
