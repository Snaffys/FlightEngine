#include <Buffers/VAO.h>

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

PosNormTexColVao::PosNormTexColVao(VBO& vbo) {
	glGenVertexArrays(1, &id);
	Bind();
	vbo.Bind();

	unsigned int ind = 0;
	// Vertex
	glEnableVertexAttribArray(ind);
	glVertexAttribPointer(ind++, sizeof(std::declval<PosNormTexCol>().pos) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(PosNormTexCol), (void*)0);
	// Normals
	glEnableVertexAttribArray(ind);
	glVertexAttribPointer(ind++, sizeof(std::declval<PosNormTexCol>().normals) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(PosNormTexCol), (void*)offsetof(PosNormTexCol, normals));
	// Texture
	glEnableVertexAttribArray(ind);
	glVertexAttribPointer(ind++, sizeof(std::declval<PosNormTexCol>().texCoords) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(PosNormTexCol), (void*)offsetof(PosNormTexCol, texCoords));
	// Color
	glEnableVertexAttribArray(ind);
	glVertexAttribPointer(ind, sizeof(std::declval<PosNormTexCol>().color) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(PosNormTexCol), (void*)offsetof(PosNormTexCol, color));
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

PosTex3DVao::PosTex3DVao(VBO& VBO) {
	glGenVertexArrays(1, &id);
	Bind();
	VBO.Bind();

	unsigned int ind = 0;
	// Vertex
	glEnableVertexAttribArray(ind);
	glVertexAttribPointer(ind++, sizeof(std::declval<PosTex3D>().pos) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(PosTex3D), (void*)0);
	// Texture
	glEnableVertexAttribArray(ind);
	glVertexAttribPointer(ind, sizeof(std::declval<PosTex3D>().texCoords) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(PosTex3D), (void*)offsetof(PosTex3D, texCoords));
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


TerrainVao::TerrainVao(const VBO& VBO) {
	//TerrainVao::TerrainVao(const VBO & VBO) {

	//glGenVertexArrays(1, &id);
	//Bind();
	//VBO.Bind();
	//// Vertex
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//VBO.Unbind();

	glGenVertexArrays(1, &id);
	Bind();
	VBO.Bind();
	unsigned int ind = 0;
	// Vertex
	glEnableVertexAttribArray(ind);
	//glVertexAttribPointer(ind++, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribPointer(ind++, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//// Normals
	glEnableVertexAttribArray(ind);
	glVertexAttribPointer(ind++, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(VBO.POSITIONS));
	VBO.Unbind();
}


ParticleVAO::ParticleVAO(VBO& quadVBO, VBO& instanceVBO) {
	glGenVertexArrays(1, &id);
	Bind();

	// ---- Static Quad Attributes ----
	quadVBO.Bind();
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(PosTex3D), (void*)0);
	// UVs
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(PosTex3D), (void*)offsetof(PosTex3D, texCoords));

	// ---- Instanced Particle Attributes ----
	instanceVBO.Bind();

	// Position (vec3)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(Particle), (void*)offsetof(Particle, position));
	glVertexAttribDivisor(2, 1);

	// Velocity (vec3)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
		sizeof(Particle), (void*)offsetof(Particle, velocity));
	glVertexAttribDivisor(3, 1);

	// Color Start (vec4)
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE,
		sizeof(Particle), (void*)offsetof(Particle, colorStart));
	glVertexAttribDivisor(4, 1);

	// Color End (vec4)
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE,
		sizeof(Particle), (void*)offsetof(Particle, colorEnd));
	glVertexAttribDivisor(5, 1);

	// Size Start (float)
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE,
		sizeof(Particle), (void*)offsetof(Particle, sizeStart));
	glVertexAttribDivisor(6, 1);

	// Size End (float)
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE,
		sizeof(Particle), (void*)offsetof(Particle, sizeEnd));
	glVertexAttribDivisor(7, 1);

	// Life (float)
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE,
		sizeof(Particle), (void*)offsetof(Particle, life));
	glVertexAttribDivisor(8, 1);

	// Rotation (float)
	glEnableVertexAttribArray(9);
	glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE,
		sizeof(Particle), (void*)offsetof(Particle, rotation));
	glVertexAttribDivisor(9, 1);

	quadVBO.Unbind();
	instanceVBO.Unbind();
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

void VAO::Delete() {
	if (id != 0) {
		glDeleteVertexArrays(1, &id);
		id = 0;
	}
}

VAO::~VAO() {
	//Delete();
}