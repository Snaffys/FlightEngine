#include "Mesh.h"

Mesh::Mesh(std::vector<PosNormTex>& vertexData, std::vector<unsigned int>& indices, std::vector<Texture>& textures) {
    this->vertexData = vertexData;
    this->indices = indices;
    this->textures = textures;

    vbo = VBO(vertexData);
    vao = VAO(vbo);
    ibo = IBO(indices);
}

void Mesh::Draw(Shader& shader, Camera& camera, glm::mat4 matrix, glm::vec3 translation,
    glm::quat rotation, glm::vec3 scale) {
    shader.Activate();
    vao.Bind();

    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;
    for (unsigned int i = 0; i < textures.size(); i++) {
        std::string num;
        std::string type = textures[i].GetType();
        if (type == "diffuse") {
            num = std::to_string(numDiffuse++);
        }
        else if (type == "specular")
        {
            num = std::to_string(numSpecular++);
        }
        shader.setInt((type + num).c_str(), i);
        textures[i].Bind();
    }
    shader.setVec3("camPos", camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
    //shader.setMat4("camMatrix", camera.getViewMatrix());
    //camera.getViewMatrix(shader, "camMatrix");
    shader.setMat4("projection", glm::perspective(glm::radians(90.0f), (float)1024 / 1024, 0.1f, 10000000.0f));
    shader.setMat4("view", camera.getViewMatrix());

    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    glm::mat4 sca = glm::mat4(1.0f);

    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);

    shader.setMat4("translation", trans);
    shader.setMat4("rotation", rot);
    shader.setMat4("scale", sca);
    shader.setMat4("model", matrix);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}