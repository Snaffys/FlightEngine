#include "Model/Model.h"

Model::Model(const char* file) {
    // load model
    
    std::string extension = std::filesystem::path(file).extension().string();
    bool loaded;
    if (extension == ".gltf") {
        loaded = loader.LoadASCIIFromFile(&model, &err, &warn, file);
    }
    else if (extension == ".glb") {
        loaded = loader.LoadBinaryFromFile(&model, &err, &warn, file);
    }
    else {
        std::cerr << "Unsupported file format!" << std::endl;
        exit(1);
    }

    if (!warn.empty()) {
        std::cout << "WARNING: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << "ERROR: " << err << std::endl;
    }
    if (!loaded) {
        std::cerr << "Failed to load glTF file." << std::endl;
        exit(1);
    }

    if (model.defaultScene == -1) {
        if (!model.scenes.empty()) {
            std::cout << "WARNING: No default scene defined. Using the first scene as fallback." << std::endl;
            model.defaultScene = 0;
        }
        else {
            std::cerr << "ERROR: No scenes found in the glTF file." << std::endl;
            exit(1);
        }
    }



    // transformations
    nodeTransforms.resize(model.nodes.size());
    for (int sceneIndex : model.scenes[model.defaultScene].nodes)
        traverseNodeHierarchy(model, sceneIndex, glm::mat4(1.0), nodeTransforms);

    // buffers
    for (int sceneIndex : model.scenes[model.defaultScene].nodes)
        processNodeMeshes(sceneIndex);

    // textures
    for (const auto& texture : model.textures) {
        const auto& image = model.images[texture.source];
        textures.push_back(Texture(image, model, file));
    }

    setupTextures();
}

void Model::processNodeMeshes(int nodeIndex) {
    const tinygltf::Node& node = model.nodes[nodeIndex];
    if (node.mesh >= 0) {
        const tinygltf::Mesh& gltfMesh = model.meshes[node.mesh];
        for (const auto& primitive : gltfMesh.primitives)
            processMeshPrimitive(primitive, nodeIndex);
    }


    for (int childIndex : node.children)
        processNodeMeshes(childIndex);
}

void Model::setupTextures() {
    for (const auto& material : model.materials) {
        Material mat;
        // 1. Base Color (Albedo)
        if (material.pbrMetallicRoughness.baseColorTexture.index > -1) {
            mat.baseColorTextureIndex = material.pbrMetallicRoughness.baseColorTexture.index;
            mat.hasBaseColorTexture = true;
        }
        else {
            mat.baseColorFactor = glm::vec4(
                material.pbrMetallicRoughness.baseColorFactor[0],
                material.pbrMetallicRoughness.baseColorFactor[1],
                material.pbrMetallicRoughness.baseColorFactor[2],
                material.pbrMetallicRoughness.baseColorFactor[3]
            );
            mat.hasBaseColorTexture = false;
        }

        // Metallic-Roughness
        if (material.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
            // Standard combined metallic-roughness texture
            mat.metallicRoughnessTextureIndex = material.pbrMetallicRoughness.metallicRoughnessTexture.index;
            mat.hasMetallicRoughnessTexture = true;
        }
        else {
            // Check for separate metallic and roughness textures (non-standard)
            if (material.extensions.find("CUSTOM_SEPARATE_METALLIC_ROUGHNESS") != material.extensions.end()) {
                auto& ext = material.extensions.at("CUSTOM_SEPARATE_METALLIC_ROUGHNESS");

                if (ext.Has("metallicTexture")) {
                    mat.metallicTextureIndex = ext.Get("metallicTexture").Get<int>();
                    mat.hasMetallicTexture = true;
                }
                if (ext.Has("roughnessTexture")) {
                    mat.roughnessTextureIndex = ext.Get("roughnessTexture").Get<int>();
                    mat.hasRoughnessTexture = true;
                }
            }
        }

        mat.metallicFactor = material.pbrMetallicRoughness.metallicFactor;
        mat.roughnessFactor = material.pbrMetallicRoughness.roughnessFactor;



        // 3. Ambient Occlusion (AO)
        if (material.occlusionTexture.index > -1) {
            mat.aoTextureIndex = material.occlusionTexture.index;
            mat.hasAOTexture = true;
        }
        else {
            mat.hasAOTexture = false;
        }

        // 4. Emission
        if (material.emissiveTexture.index > -1) {
            mat.emissiveTextureIndex = material.emissiveTexture.index;
            mat.hasEmissionTexture = true;
        }
        else {
            mat.emissiveFactor = glm::vec3(
                material.emissiveFactor[0],
                material.emissiveFactor[1],
                material.emissiveFactor[2]
            );
            mat.hasEmissionTexture = false;
        }
        materials.push_back(mat);
    }
}

glm::mat4 Model::getNodeTransformMat(const tinygltf::Node & node) {
    glm::mat4 transform = glm::mat4(1.0f);

    if (!node.matrix.empty()) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                transform[i][j] = node.matrix[i * 4 + j];
            }
        }
    }
    else {
        if (!node.translation.empty()) {
            glm::vec3 translation = glm::vec3(node.translation[0], node.translation[1], node.translation[2]);
            transform = glm::translate(transform, translation);
        }
        // gltf uses quaternions
        if (!node.rotation.empty()) {
            glm::quat rotation = glm::quat(
                node.rotation[3],   // w
                node.rotation[0],   // x
                node.rotation[1],   // y
                node.rotation[2]    // z
            );
            transform *= glm::mat4_cast(rotation);
        }
        if (!node.scale.empty()) {
            glm::vec3 scale = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
            transform = glm::scale(transform, scale);
        }
    }
    return transform;
}


void Model::traverseNodeHierarchy(const tinygltf::Model & model, int nodeIndex, const glm::mat4 & parentTransform, std::vector<glm::mat4>&nodeTransforms) {
    const tinygltf::Node& node = model.nodes[nodeIndex];

    // transformation relative to the parent
    glm::mat4 localTransform = getNodeTransformMat(node);

    glm::mat4 globalTranform = parentTransform * localTransform;

    nodeTransforms[nodeIndex] = globalTranform;

    for (int childIndex : node.children) {
        traverseNodeHierarchy(model, childIndex, globalTranform, nodeTransforms);
    }
}

void Model::processMeshPrimitive(const tinygltf::Primitive& primitive, int nodeIndex) {
    ModelMesh newMesh;
    newMesh.materialIndex = primitive.material;
    newMesh.nodeIndex = nodeIndex;

    // Vertex positions
    const auto& positionsAccessor = model.accessors[primitive.attributes.at("POSITION")];
    const auto& positionsBufferView = model.bufferViews[positionsAccessor.bufferView];
    const auto& positionsBuffer = model.buffers[positionsBufferView.buffer];

    size_t stride = positionsBufferView.byteStride ? positionsBufferView.byteStride : sizeof(float) * 3;

    std::vector<glm::vec3> positions;
    for (int i = 0; i < positionsAccessor.count; ++i) {
        const float* posPtr = reinterpret_cast<const float*>(&positionsBuffer.data[positionsBufferView.byteOffset + positionsAccessor.byteOffset + i * stride]);
        positions.emplace_back(posPtr[0], posPtr[1], posPtr[2]);
    }

    // Normals, Colors, and UVs (optional)
    const float* normals = nullptr;
    const float* colors = nullptr;
    const float* uvs = nullptr;

    if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
        const auto& normalsAccessor = model.accessors[primitive.attributes.at("NORMAL")];
        const auto& normalsBufferView = model.bufferViews[normalsAccessor.bufferView];
        const auto& normalsBuffer = model.buffers[normalsBufferView.buffer];
        normals = reinterpret_cast<const float*>(&normalsBuffer.data[normalsBufferView.byteOffset + normalsAccessor.byteOffset]);
    }

    if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
        const auto& colorsAccessor = model.accessors[primitive.attributes.at("COLOR_0")];
        const auto& colorsBufferView = model.bufferViews[colorsAccessor.bufferView];
        const auto& colorsBuffer = model.buffers[colorsBufferView.buffer];
        colors = reinterpret_cast<const float*>(&colorsBuffer.data[colorsBufferView.byteOffset + colorsAccessor.byteOffset]);
    }

    if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
        const auto& uvsAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
        const auto& uvsBufferView = model.bufferViews[uvsAccessor.bufferView];
        const auto& uvsBuffer = model.buffers[uvsBufferView.buffer];
        uvs = reinterpret_cast<const float*>(&uvsBuffer.data[uvsBufferView.byteOffset + uvsAccessor.byteOffset]);
    }

    // INDICES_________________________________________________________________________________________________________
    if (primitive.indices >= 0) {
        const auto& indicesAccessor = model.accessors[primitive.indices];
        const auto& indicesBufferView = model.bufferViews[indicesAccessor.bufferView];
        const auto& indicesBuffer = model.buffers[indicesBufferView.buffer];

        const void* indicesData = &indicesBuffer.data[indicesBufferView.byteOffset + indicesAccessor.byteOffset];
        std::vector<unsigned int> loadedIndices;

        switch (indicesAccessor.componentType) {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            for (size_t i = 0; i < indicesAccessor.count; ++i)
                loadedIndices.push_back(static_cast<unsigned int>(reinterpret_cast<const uint8_t*>(indicesData)[i]));
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            for (size_t i = 0; i < indicesAccessor.count; ++i)
                loadedIndices.push_back(static_cast<unsigned int>(reinterpret_cast<const uint16_t*>(indicesData)[i]));
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            for (size_t i = 0; i < indicesAccessor.count; ++i)
                loadedIndices.push_back(reinterpret_cast<const uint32_t*>(indicesData)[i]);
            break;
        default:
            std::cerr << "Unsupported index type!" << std::endl;
            return;
        }

        newMesh.indices = loadedIndices;
    }

    // VERTICES_________________________________________________________________________________________________
    for (size_t i = 0; i < positionsAccessor.count; ++i) {
        const float* posPtr = reinterpret_cast<const float*>(&positionsBuffer.data[positionsBufferView.byteOffset + positionsAccessor.byteOffset + i * stride]);
        glm::vec3 pos(posPtr[0], posPtr[1], posPtr[2]);

        glm::vec3 normal = normals ? glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]) : glm::vec3(0.0f);
        glm::vec3 color = colors ? glm::vec3(colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2]) : glm::vec3(1.0f);
        glm::vec2 uv = uvs ? glm::vec2(uvs[i * 2], uvs[i * 2 + 1]) : glm::vec2(0.0f);

        newMesh.vertices.emplace_back(pos, normal, uv, color);
    }

    // NORMALS__________________________________________________________________________________________________
    if (!normals && !newMesh.indices.empty()) {
        for (size_t i = 0; i < newMesh.indices.size(); i += 3) {
            unsigned int i0 = newMesh.indices[i];
            unsigned int i1 = newMesh.indices[i + 1];
            unsigned int i2 = newMesh.indices[i + 2];

            glm::vec3 v0 = newMesh.vertices[i0].pos;
            glm::vec3 v1 = newMesh.vertices[i1].pos;
            glm::vec3 v2 = newMesh.vertices[i2].pos;

            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

            newMesh.vertices[i0].normals += normal;
            newMesh.vertices[i1].normals += normal;
            newMesh.vertices[i2].normals += normal;
        }

        for (auto& vertex : newMesh.vertices) {
            vertex.normals = glm::normalize(vertex.normals);
        }
        std::cout << "Generated normals using indices.\n";
    }

    // Create VBO (using make_unique)
    newMesh.vbo = std::make_unique<VBO>(newMesh.vertices);

    // Create VAO using the VBO (pass raw pointer with get())
    newMesh.vao = std::make_unique<PosNormTexColVao>(*newMesh.vbo); // Dereference with *

    // Create IBO if needed
    if (!newMesh.indices.empty()) {
        newMesh.ibo = std::make_unique<IBO>(newMesh.indices);
    }

    meshes.push_back(std::move(newMesh));
    
    
}

void Model::renderModel(Shader & shaderProgram, glm::mat4 view, glm::mat4 projection, bool freeCam, glm::vec3 planePosition, glm::quat rotation, glm::mat4 scaleMatrix) {
    shaderProgram.Activate();

    // Convert quaternion to rotation matrix
    glm::mat4 yawRotationMatrix = glm::mat4_cast(rotation);

    for (auto& mesh : meshes) {
        const Material& mat = materials[mesh.materialIndex];



        // Send Base Color (Albedo)
        if (mat.hasBaseColorTexture) {
            shaderProgram.setInt("baseColorTexture", textures[mat.baseColorTextureIndex].GetId());
            shaderProgram.setBool("hasBaseColorTexture", true);
        }
        else {
            shaderProgram.setVec4("baseColorFactor", mat.baseColorFactor);
            shaderProgram.setBool("hasBaseColorTexture", false);
        }

        if (mat.hasMetallicRoughnessTexture) {
            shaderProgram.setInt("metaillicRoughnessTexture", textures[mat.metallicRoughnessTextureIndex].GetId());
            shaderProgram.setBool("hasMetaillicRoughnessTexture", true);
        }
        else {
            shaderProgram.setBool("hasMetaillicRoughnessTexture", false);

            // Send Metallic Texture
            if (mat.hasMetallicTexture) {
                shaderProgram.setInt("metallicTexture", textures[mat.metallicTextureIndex].GetId());
                shaderProgram.setBool("hasMetallicTexture", true);
            }
            else {
                shaderProgram.setBool("hasMetallicTexture", false);
            }

            // Send Roughness Texture
            if (mat.hasRoughnessTexture) {
                shaderProgram.setInt("roughnessTexture", textures[mat.roughnessTextureIndex].GetId());
                shaderProgram.setBool("hasRoughnessTexture", true);
            }
            else {
                shaderProgram.setBool("hasRoughnessTexture", false);
            }
        }

        shaderProgram.setFloat("metallicFactor", mat.metallicFactor);
        shaderProgram.setFloat("roughnessFactor", mat.roughnessFactor);

        // Send Ambient Occlusion (AO)
        if (mat.hasAOTexture) {
            shaderProgram.setInt("aoTexture", textures[mat.aoTextureIndex].GetId());
            shaderProgram.setBool("hasAOTexture", true);
        }
        else {
            shaderProgram.setBool("hasAOTexture", false);
        }

        // Send Emissive Map
        if (mat.hasEmissionTexture) {
            shaderProgram.setInt("emissiveTexture", textures[mat.emissiveTextureIndex].GetId());
            shaderProgram.setBool("hasEmissionTexture", true);
        }
        else {
            shaderProgram.setVec3("emissiveFactor", mat.emissiveFactor);
            shaderProgram.setBool("hasEmissionTexture", false);
        }



        glm::mat4 meshTransform = nodeTransforms[mesh.nodeIndex];

        if (freeCam) {
            meshTransform = glm::translate(glm::mat4(1.0f), planePosition) * scaleMatrix * yawRotationMatrix * meshTransform;
        }
        else {
            meshTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -10.0f, -30.0f)) * scaleMatrix * yawRotationMatrix * meshTransform;
            view = glm::mat4(1.0);
        }


        //meshTransform = rootTransform * meshTransform;

        shaderProgram.setMat4("projection", projection);
        shaderProgram.setMat4("view", view);

        shaderProgram.setMat4("transform", meshTransform);

        //if (freeCam) {
        //    glm::mat4 model = glm::translate(glm::mat4(1.0), planePosition);
        //    shaderProgram.setMat4("transform", transform);
        //    shaderProgram.setMat4("projection", projection);
        //    shaderProgram.setMat4("view", view);
        //}
        //else {
        //    glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -10.0f, -30.0f));
        //    shaderProgram.setMat4("transform", transform);
        //    shaderProgram.setMat4("projection", projection);
        //    shaderProgram.setMat4("view", glm::mat4(1.0f));
        //}

        //shaderProgram.setMat4("projection", projection);
        //shaderProgram.setMat4("view", view);
        //shaderProgram.setMat4("transform", transform);




        //shaderProgram.setInt("textureSampler", textures.empty() ? 0 : textures[0].GetId());
        mesh.vao->Bind();
        if (!mesh.indices.empty()) {
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
        }
    }
}