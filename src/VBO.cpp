#include <VBO.h>

VBO::VBO(std::initializer_list<PosNormTex> coords) : posNormTexVec(coords) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, posNormTexVec.size() * sizeof(PosNormTex), &posNormTexVec[0], GL_STATIC_DRAW);
}

VBO::VBO(std::initializer_list<PosTex2D> coords) : posTex2DVec(coords) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, posTex2DVec.size() * sizeof(PosTex2D), &posTex2DVec[0], GL_STATIC_DRAW);
}

VBO::VBO(std::initializer_list<Pos3D> coords) : pos3DVec(coords) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, pos3DVec.size() * sizeof(Pos3D), &pos3DVec[0], GL_STATIC_DRAW);
}

VBO::VBO(std::vector<float> vertices) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
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

VBO::~VBO() {
    //Delete();
}