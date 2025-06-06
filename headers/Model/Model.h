#ifndef MODEL_H
#define MODEL_H

#include<Model/tiny_gltf.h>
#include"Model/Mesh.h"
#include"Buffers/VBO.h"
#include <glm/gtx/matrix_decompose.hpp>

//struct Mesh {
//    //PosNormTexColVao vao;
//    //VBO vbo;
//    //IBO ibo;
//
//    //std::vector<PosNormTexCol> vertices;
//    //std::vector<unsigned int> indices;
//    //
//    //std::vector<Texture> textures;
//
//
//    //Texture baseColorTexture;
//    //Texture metallicRoughnessTexture;
//    //Texture metallicTexture;
//    //Texture roughnessTexture;
//    //Texture aoTexture;
//    //Texture emissiveTexture;
//
//    //bool hasMetallicTexture = false;
//    //bool hasRoughnessTexture = false;
//    //bool hasSeparateMetallic = false;
//    //bool hasSeparateRoughness = false;
//    //bool hasBaseColorTexture;
//    //bool hasMetallicRoughnessTexture;
//    //bool hasAOTexture;
//    //bool hasEmissionTexture;
//
//    //float metallicFactor = 1.0;
//    //float roughnessFactor = 1.0;
//
//    //glm::vec4 baseColorFactor;
//    //glm::vec3 emissiveFactor;
//};

//struct ModelMesh {
//    PosNormTexColVao vao;
//    VBO vbo;
//    IBO ibo;
//    std::vector<unsigned int> indices;
//    std::vector<PosNormTexCol> vertices;
//    int materialIndex; // Index into Model's materials vector
//    int nodeIndex;
//};

struct ModelMesh {
    std::unique_ptr<PosNormTexColVao> vao;
    std::unique_ptr<VBO> vbo;
    std::unique_ptr<IBO> ibo;
    std::vector<unsigned int> indices;
    std::vector<PosNormTexCol> vertices;
    int materialIndex;
    int nodeIndex;

    // Enable move operations
    ModelMesh(ModelMesh&& other) noexcept :
        vao(std::move(other.vao)),
        vbo(std::move(other.vbo)),
        ibo(std::move(other.ibo)),
        indices(std::move(other.indices)),
        vertices(std::move(other.vertices)),
        materialIndex(other.materialIndex),
        nodeIndex(other.nodeIndex)
    {}

    ModelMesh& operator=(ModelMesh&& other) noexcept {
        if (this != &other) {
            vao = std::move(other.vao);
            vbo = std::move(other.vbo);
            ibo = std::move(other.ibo);
            indices = std::move(other.indices);
            vertices = std::move(other.vertices);
            materialIndex = other.materialIndex;
            nodeIndex = other.nodeIndex;
        }
        return *this;
    }

    // Delete copy operations
    ModelMesh(const ModelMesh&) = delete;
    ModelMesh& operator=(const ModelMesh&) = delete;

    // Default constructor
    ModelMesh() = default;
};

struct Material {
    int baseColorTextureIndex = -1; // Index into the `textures` vector
    glm::vec4 baseColorFactor = glm::vec4(1.0f);
    bool hasBaseColorTexture = false;

    int metallicTextureIndex = -1;
    int roughnessTextureIndex = -1;
    int metallicRoughnessTextureIndex = -1;
    float metallicFactor = 0.0f;
    float roughnessFactor = 0.0f;
    bool hasMetallicTexture = false;
    bool hasRoughnessTexture = false;
    bool hasMetallicRoughnessTexture = false;

    int aoTextureIndex = -1;
    bool hasAOTexture = false;

    int emissiveTextureIndex = -1;
    glm::vec3 emissiveFactor = glm::vec3(0.0f);
    bool hasEmissionTexture = false;

    int diffuseTextureIndex = -1;
    glm::vec4 diffuseFactor = glm::vec4(1.0f);
    int specularGlossinessTextureIndex = -1;
    glm::vec3 specularFactor = glm::vec3(1.0f);
    float glossinessFactor = 1.0f;

};

class Model {
public:
    Model() = default;
    Model(const char* file);

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    std::vector<glm::mat4> nodeTransforms;
    std::vector<Texture> textures;

    //std::vector<Mesh> meshes;
   
    void setupTextures();

    std::vector<int> primitiveMaterialIndices;



    std::vector<Material> materials;



    std::vector<ModelMesh> meshes;


    //std::vector<PosNormTexCol> vertices;
    //std::vector<unsigned int> indices;




    void renderModel(Shader& shaderProgram, glm::mat4 view, glm::mat4 projection, bool freeCam, glm::vec3 planePosition, const tinygltf::Node& node);
    void renderModel(Shader& shaderProgram, glm::mat4 view, glm::mat4 projection, bool freeCam, glm::vec3 planePosition, glm::quat rotation, glm::mat4 scaleMatrix = glm::mat4(1.0));
    //void renderModel();
private:

    void processNodeMeshes(int nodeIndex);

    void processModel();
    void processMeshPrimitive(const tinygltf::Primitive& primitive, int nodeIndex);
    glm::mat4 getNodeTransformMat(const tinygltf::Node& node);
    void traverseNodeHierarchy(const tinygltf::Model& model, int nodeIndex, const glm::mat4& parentTransform, std::vector<glm::mat4>& nodeTransforms);
};

#endif