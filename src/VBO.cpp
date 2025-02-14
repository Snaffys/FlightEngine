#include <VBO.h>

VBO::VBO(std::initializer_list<PosNormTex> coords) : posNormTexVec(coords) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, posNormTexVec.size() * sizeof(PosNormTex), &posNormTexVec[0], GL_STATIC_DRAW);
    vertexCount = posNormTexVec.size();
}

VBO::VBO(std::vector<PosNormTex>& coords) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, posNormTexVec.size() * sizeof(PosNormTex), coords.data(), GL_STATIC_DRAW);
    vertexCount = posNormTexVec.size();
}

VBO::VBO(std::initializer_list<PosTex2D> coords) : posTex2DVec(coords) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, posTex2DVec.size() * sizeof(PosTex2D), &posTex2DVec[0], GL_STATIC_DRAW);
    vertexCount = posTex2DVec.size();
}

VBO::VBO(std::initializer_list<Pos3D> coords) : pos3DVec(coords) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, pos3DVec.size() * sizeof(Pos3D), &pos3DVec[0], GL_STATIC_DRAW);
    vertexCount = pos3DVec.size();
}

VBO::VBO(std::vector<float> vertices) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
    vertexCount = vertices.size();
}

VBO::VBO(std::vector<glm::vec3> vecVertices) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(vecVertices[0]), &vecVertices[0], GL_STATIC_DRAW);
    vertexCount = vecVertices.size();
}

//VBO::VBO(std::vector<float> positions, std::vector<float> normals) {
//
//    //glGenBuffers(1, &id);
//    //Bind();
//    //glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);
//    //vertexCount = positions.size();
//
//
//    glGenBuffers(1, &id);
//    Bind();
//    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]) + normals.size() * sizeof(normals[0]), nullptr, GL_STATIC_DRAW);
//    glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(positions[0]), &positions[0]);
//    glBufferSubData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]), normals.size() * sizeof(normals[0]), &normals[0]);
//    vertexCount = sizeof(positions) / sizeof(float) / 3;
//    POSITIONS = positions.size() * sizeof(positions[0]);
//}


VBO::VBO(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals) {

    //glGenBuffers(1, &id);
    //Bind();
    //glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);
    //vertexCount = positions.size();


    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]) + normals.size() * sizeof(normals[0]), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(positions[0]), &positions[0]);
    glBufferSubData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]), normals.size() * sizeof(normals[0]), &normals[0]);
    vertexCount = sizeof(positions) / sizeof(float) / 3;
    POSITIONS = positions.size() * sizeof(positions[0]);
}




void VBO::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VBO::Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete() {
    Unbind();
    glDeleteBuffers(1, &id);
}

unsigned int VBO::getVertexCount() {
    return vertexCount;
}

VBO::~VBO() {
    //Delete();
}