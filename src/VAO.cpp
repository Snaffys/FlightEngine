#include <VAO.h>

VAO::VAO(VBO& vbo) {
	glGenVertexArrays(1, &id);
	Bind();
	vbo.Bind();

	unsigned int ind = 0;
	// Vertex
	glEnableVertexAttribArray(ind);
	glVertexAttribPointer(ind++, sizeof(std::declval<PosNormTex>().pos) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(PosNormTex), (void*)0);
	// Normals
	glEnableVertexAttribArray(ind);
	glVertexAttribPointer(ind++, sizeof(std::declval<PosNormTex>().normals) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(PosNormTex), (void*)offsetof(PosNormTex, normals));
	// Texture
	glEnableVertexAttribArray(ind);
	glVertexAttribPointer(ind, sizeof(std::declval<PosNormTex>().texCoords) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(PosNormTex), (void*)offsetof(PosNormTex, texCoords));
	vbo.Unbind();
}

PosTex2DVao::PosTex2DVao(VBO& VBO) {
	glGenVertexArrays(1, &id);
	Bind();
	VBO.Bind();

	unsigned int ind = 0;
	// Vertex
	glEnableVertexAttribArray(ind);
	glVertexAttribPointer(ind++, sizeof(std::declval<PosTex2D>().pos) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(PosTex2D), (void*)0);
	// Texture
	glEnableVertexAttribArray(ind);
	glVertexAttribPointer(ind, sizeof(std::declval<PosTex2D>().texCoords) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(PosTex2D), (void*)offsetof(PosTex2D, texCoords));
	VBO.Unbind();
}

Pos3DVao::Pos3DVao(VBO& VBO) {
	glGenVertexArrays(1, &id);
	Bind();
	VBO.Bind();

	// Vertex
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, sizeof(std::declval<Pos3D>().pos) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Pos3D), (void*)0);
	VBO.Unbind();
}


TerrainVao::TerrainVao(VBO& VBO) {
	glGenVertexArrays(1, &id);
	Bind();
	VBO.Bind();
	// Vertex
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	VBO.Unbind();
}

bool VAO::IsValid() {
	return glIsVertexArray(id);
}

void VAO::Bind() {
	glBindVertexArray(id);
}

void VAO::Unbind() {
	glBindVertexArray(0);
}

void VAO::Delete()
{
	Unbind();
	glDeleteBuffers(1, &id);
}

VAO::~VAO() {
	//Delete();
}