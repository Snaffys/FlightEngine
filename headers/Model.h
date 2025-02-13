#ifndef MODEL_H
#define MODEL_H

#include<json/json.h>
#include"Mesh.h"

using json = nlohmann::json;

class Model {
public:
    Model() = default;
    Model(const char* file);
    void Draw(Shader& shader, Camera& camera);
private:
    const char* file;
    std::vector<unsigned char> data;
    json JSON;

    std::vector<std::string> loadedTexName;
    std::vector<Texture> loadedTex;

    std::vector<Mesh> meshes;
    std::vector<glm::vec3> translationMeshes;
    std::vector<glm::quat> rotationMeshes;
    std::vector<glm::vec3> scaleMeshes;
    std::vector<glm::mat4> matrixMeshes;

    void loadMesh(unsigned int indMesh);

    void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0));

    std::vector<unsigned char> getData();
    std::vector<float> getFloats(json accessor);
    std::vector<unsigned int> getIndices(json accessor);
    std::vector<Texture> getTextures();

    std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
    std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);
	std::vector<glm::vec4> groupFloatsVec4(std::vector<float> floatVec);


    std::vector<PosNormTex> assembleVertices
    (
        std::vector<glm::vec3> positions,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> texUVs
    );

    char* getFileContents(const char* filename);
};

#endif
