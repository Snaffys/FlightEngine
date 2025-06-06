//#include "Terrain/Terrain.h"
//
//Terrain::Terrain(Camera* camera, unsigned int vertexWidthAmount, unsigned int vertexHeightAmount, unsigned int octaveCount, float scalingBias) {
//    this->camera = camera;
//    this->vertexWidthAmount = vertexWidthAmount;
//    this->vertexHeightAmount = vertexHeightAmount;
//    this->octaveCount = octaveCount;
//    this->scalingBias = scalingBias;
//
//    //double distToChunkEdge = sqrt((vertexWidthAmount - 1) * (vertexWidthAmount - 1) + (vertexHeightAmount - 1) * (vertexHeightAmount - 1));
//    double distToChunkEdge = (vertexWidthAmount - 1);
//    detailLevels = { LODInfo(0, distToChunkEdge * 1)};
//    //detailLevels = { LODInfo(0, distToChunkEdge * 6), LODInfo(4, distToChunkEdge * 7), LODInfo(8, distToChunkEdge * 8) };
//    //detailLevels = { LODInfo(0, distToChunkEdge * 4), LODInfo(1,distToChunkEdge * 6), LODInfo(2,distToChunkEdge * 8) };
//    this->maxViewDst = detailLevels[detailLevels.size() - 1].visibleDstThreshold;
//    this->chunkSize = vertexWidthAmount - 1;
//    //this->chunksVisibleInViewDst = int(maxViewDst) / chunkSize;
//    this->chunksVisibleInViewDst = static_cast<int>(std::ceil(maxViewDst / chunkSize));
//}
//
//Terrain::Terrain(const Terrain& other) : camera(other.camera), vertexWidthAmount(other.vertexWidthAmount), vertexHeightAmount(other.vertexHeightAmount),
//octaveCount(other.octaveCount), scalingBias(other.scalingBias), maxViewDst(other.maxViewDst),
//chunkSize(other.chunkSize), chunksVisibleInViewDst(other.chunksVisibleInViewDst), chunks(other.chunks),
//visibleChunks(other.visibleChunks), mapDataThreadInfoQueue(other.mapDataThreadInfoQueue),
//detailLevels(other.detailLevels) {}
//
//Terrain& Terrain::operator=(const Terrain& other) {
//    if (this != &other) {
//        camera = other.camera;
//        vertexWidthAmount = other.vertexWidthAmount;
//        vertexHeightAmount = other.vertexHeightAmount;
//        octaveCount = other.octaveCount;
//        scalingBias = other.scalingBias;
//        maxViewDst = other.maxViewDst;
//        chunkSize = other.chunkSize;
//        chunksVisibleInViewDst = other.chunksVisibleInViewDst;
//        chunks = other.chunks;
//        visibleChunks = other.visibleChunks;
//        mapDataThreadInfoQueue = other.mapDataThreadInfoQueue;
//        detailLevels = other.detailLevels;
//    }
//    return *this;
//}
//
//void Terrain::updateVisibleChunks() {
//    float camPosX = camera->getCameraPosition().x;
//    float camPosZ = camera->getCameraPosition().z;
//
//    // Calculate current chunk using FLOOR (not round)
//    int currentChunkCoordX = static_cast<int>(std::floor(camPosX / chunkSize));
//    int currentChunkCoordZ = static_cast<int>(std::floor(camPosZ / chunkSize));
//
//    std::lock_guard<std::mutex> lock(queueMutex);
//
//    //std::cout << currentChunkCoordX << "\t" << currentChunkCoordZ << std::endl;
//
//    // Generate chunks in a square grid around the camera
//    for (int yOffset = -chunksVisibleInViewDst; yOffset <= chunksVisibleInViewDst; yOffset++) { // Use <=
//        for (int xOffset = -chunksVisibleInViewDst; xOffset <= chunksVisibleInViewDst; xOffset++) { // Use <=
//            glm::vec2 viewedChunkCoord(currentChunkCoordX + xOffset, currentChunkCoordZ + yOffset);
//            if (chunks.find(viewedChunkCoord) == chunks.end()) {
//                chunks.emplace(viewedChunkCoord,
//                    Chunk(vertexWidthAmount, vertexHeightAmount, octaveCount,
//                        scalingBias, viewedChunkCoord, detailLevels));
//            }
//        }
//    }
//
//    // Update visibility for ALL chunks
//    for (auto& [coord, chunk] : chunks) {
//        updateChunk(chunk, currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
//    }
//}
//
//void Terrain::updateChunk(Chunk& chunk, int currentChunkCoordX, int currentChunkCoordZ, float camPosX, float camPosZ) {
//    glm::vec2 chunkPos = chunk.getPosition();
//
//    // World position of chunk's edges
//    float xMin = chunkPos.x * chunkSize;
//    float xMax = xMin + chunkSize;
//    float zMin = chunkPos.y * chunkSize;
//    float zMax = zMin + chunkSize;
//
//    // Distance to the NEAREST edge (left/right for X, top/bottom for Z)
//    float dx = std::max(0.0f, std::max(xMin - camPosX, camPosX - xMax));
//    float dz = std::max(0.0f, std::max(zMin - camPosZ, camPosZ - zMax));
//
//    // Euclidean distance to the nearest edge
//    float distanceToNearestEdge = std::sqrt(dx * dx + dz * dz);
//
//    bool visible = (distanceToNearestEdge <= maxViewDst);
//
//    if (visible) {
//        int lodIndex = 0;
//        for (int i = 0; i < detailLevels.size() - 1; i++) {
//            if (distanceToNearestEdge > detailLevels[i].visibleDstThreshold) {
//                lodIndex = i + 1;
//            }
//            else {
//                break;
//            }
//        }
//
//        if (lodIndex != chunk.getPreviousLodIndex()) {
//            if (chunk.getLodChunk(lodIndex).getHasChunk()) {
//                chunk.setPreviousLodIndex(lodIndex);
//                chunk.setVBO(chunk.getLodChunk(lodIndex).getChunk()->getVBO());
//                chunk.setVAO(chunk.getLodChunk(lodIndex).getChunk()->getVAO());
//                chunk.setIBO(chunk.getLodChunk(lodIndex).getChunk()->getIBO());
//                chunk.setNumStrips(chunk.getLodChunk(lodIndex).getChunk()->getNumStrips());
//                chunk.setNumVertsPerStrip(chunk.getLodChunk(lodIndex).getChunk()->getNumVertsPerStrip());
//            }
//            else if (!chunk.getLodChunk(lodIndex).getHasRequestedChunk()) {
//                chunk.getLodChunkPtr(lodIndex)->setHasRequestedChunk(true);
//                chunkDataThreadStart(lodIndex, chunkPos, [this, &chunk, lodIndex, currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ](Chunk&) {
//                    chunk.getLodChunkPtr(lodIndex)->onChunkDataReceived(*chunk.getLodChunk(lodIndex).getChunk());
//                    updateChunk(chunk, currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
//                    });
//            }
//        }
//    }
//
//    // Update visibility status
//    if (chunk.getLoaded() != visible) {
//        if (visible) {
//            visibleChunks.push_back(&chunk);
//        }
//        else {
//            visibleChunks.erase(std::remove(visibleChunks.begin(), visibleChunks.end(), &chunk), visibleChunks.end());
//        }
//        chunk.setLoaded(visible);
//    }
//}
//
//
//
//void Terrain::updateReceivedChunk(glm::vec2 chunkPos) {
//    float camPosX = camera->getCameraPosition().x;
//    float camPosZ = camera->getCameraPosition().z;
//
//    int currentChunkCoordX = floor(camPosX / chunkSize);
//    int currentChunkCoordZ = floor(camPosZ / chunkSize);
//
//    updateChunk(chunks[chunkPos], currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
//}
//
//void LODChunk::onChunkDataReceived(Chunk& chunk) {
//    //chunk.setVBO(VBO(chunk.getVertices()));
//    chunk.setVBO(VBO(chunk.getVertices(), chunk.getNormals()));
//    chunk.setVAO(TerrainVao(chunk.getVBO()));
//    chunk.setIBO(IBO(chunk.getIndices()));
//    chunk.getVerticesPtr()->clear();
//    chunk.getIndicesPtr()->clear();
//    chunk.getNormalsPtr()->clear();
//
//    hasMesh = true;
//}
//
//void Terrain::chunkDataThreadStart(int lod, glm::vec2 pos, std::function<void(Chunk&)> callbackFunc) {
//    std::thread thread([this, lod, pos, callbackFunc]() {
//
//        Chunk chunk;
//        {
//            std::lock_guard<std::mutex> lock(queueMutex);
//            chunk = chunks[pos];
//        }
//        chunk.generateChunkData(chunks[pos].getLodChunk(lod).getLOD());
//
//        {
//            std::lock_guard<std::mutex> lock(queueMutex);
//            chunks[pos].setPerlinNoise2D(chunk.getPerlinNoise2D());
//
//            chunks[pos].getLodChunk(lod).getChunk()->setVertices(chunk.getVertices());
//            chunks[pos].getLodChunk(lod).getChunk()->setIndices(chunk.getIndices());
//            chunks[pos].getLodChunk(lod).getChunk()->setNormals(chunk.getNormals());
//            chunks[pos].getLodChunk(lod).getChunk()->setNumStrips(chunk.getNumStrips());
//            chunks[pos].getLodChunk(lod).getChunk()->setNumVertsPerStrip(chunk.getNumVertsPerStrip());
//            mapDataThreadInfoQueue.push(ChunkThreadData(callbackFunc, *chunks[pos].getLodChunk(lod).getChunk()));
//        }
//        });
//
//    thread.detach();
//}
//
//void Chunk::generateChunkData(int lod) {
//    if (perlinNoise2D == nullptr)
//        //createPerlinNoise2D();
//        GenerateNoiseMap();
//
//    unsigned int meshSimplificationIncrement = (lod == 0) ? 1 : lod * 2;
//    int verticesPerLine = ((vertexWidthAmount - 1) / meshSimplificationIncrement) + 1;
//
//    numStrips = (vertexHeightAmount - 1) / meshSimplificationIncrement;
//    numVertsPerStrip = verticesPerLine * 2;  // each strip consists of squares, each square is 2 triangles (2 lines)
//
//    // Generate vertices
//    for (unsigned int i = 0; i < vertexWidthAmount; i += meshSimplificationIncrement) {
//        for (unsigned int j = 0; j < vertexHeightAmount; j += meshSimplificationIncrement) {
//            float y = (float)((perlinNoise2D[j * vertexWidthAmount + i]));
//            vertices.push_back(glm::vec3(i, y, j));
//        }
//    }
//
//    for (unsigned int i = 0; i < verticesPerLine - 1; i++)     // for each row (strip)
//        for (unsigned int j = 0; j < verticesPerLine; j++)    // for each column
//            for (unsigned int k = 0; k < 2; k++)            // for each side of the strip (leftand right primitives in square)
//                indices.push_back(verticesPerLine * (i + k) + j);   // top left -> top right -> bottom left -> bottom right
//
//
//    normals.resize(vertices.size(), glm::vec3(0));
//    for (size_t i = 0; i + 2 < indices.size(); i++) {
//        int index1 = indices[i];
//        int index2 = indices[i + 1];
//        int index3 = indices[i + 2];
//
//        // Calculate the normal for this triangle
//        auto edge1 = vertices[index2] - vertices[index1];
//        auto edge2 = vertices[index3] - vertices[index1];
//        auto normal = glm::cross(edge1, edge2);
//
//        // Ensure the normal faces the correct direction
//        if (normal.y < 0) {
//            normal = -normal;
//        }
//
//        normals[index1] += normal;
//        normals[index2] += normal;
//        normals[index3] += normal;
//    }
//
//    for (size_t i = 0; i < normals.size(); i++) {
//        normals[i] = glm::normalize(normals[i]);
//    }
//}
//
//void Chunk::GenerateNoiseMap() {
//    this->perlinNoise2D = new float[vertexWidthAmount * vertexHeightAmount];
//
//    // random values generation
//    std::mt19937 gen(9125123);
//    std::uniform_real_distribution<float> dist(0.0, 1.0);
//    this->noiseSeed2D = new float[vertexWidthAmount * vertexHeightAmount];
//    for (unsigned int i = 0; i < vertexWidthAmount * vertexHeightAmount; i++) {
//        this->noiseSeed2D[i] = dist(gen); // [0.0, 1.0]
//    }
//
//    float chunkScale = 590.0f;
//    float amplitude = 1.0f; // controls the strength of each octave contribution scalingBias - how much amplitude decreases for each octave
//    float frequency = 1.0f; // controls details for each octave
//    float lacunarity = 2.0f; // how much frequency increases for each octave
//
//    float maxPossibleHeight = 0.0f;
//    // calculate maximum possible height for furhter normalization
//    for (int oct = 0; oct < octaveCount; oct++) {
//        maxPossibleHeight += amplitude;
//        amplitude *= scalingBias;
//    }
//
//    // generate Perlin noise
//    for (int x = 0; x < vertexWidthAmount; x++) {
//        for (int y = 0; y < vertexHeightAmount; y++) {
//            float noiseHeight = 0.0f;
//            amplitude = 1.0f;
//            frequency = 1.0f;
//
//            // Iterate through octaves
//            for (int oct = 0; oct < octaveCount; oct++) {
//                // Calculate global sample coordinates
//                float sampleX = (x + position.x * (vertexWidthAmount - 1)) / chunkScale * frequency;
//                float sampleY = (y + position.y * (vertexHeightAmount - 1)) / chunkScale * frequency;
//
//                // interpolation factors
//                float blendX = fade(sampleX - std::floor(sampleX));
//                float blendY = fade(sampleY - std::floor(sampleY));
//
//                auto wrap = [](float value, int size) {
//                    return std::fmod(std::fmod(value, size) + size, size);  // adding size avoids negative values
//                    };
//                // get close to each other points
//                int wrappedX1 = wrap(sampleX, vertexWidthAmount);
//                int wrappedX2 = wrap(sampleX + 1, vertexWidthAmount);
//                int wrappedY1 = wrap(sampleY, vertexHeightAmount);
//                int wrappedY2 = wrap(sampleY + 1, vertexHeightAmount);
//
//                // linear interpolation
//                float sampleT = lerp(noiseSeed2D[wrappedY1 * vertexWidthAmount + wrappedX1],
//                    noiseSeed2D[wrappedY1 * vertexWidthAmount + wrappedX2], blendX);
//                float sampleB = lerp(noiseSeed2D[wrappedY2 * vertexWidthAmount + wrappedX1],
//                    noiseSeed2D[wrappedY2 * vertexWidthAmount + wrappedX2], blendX);
//                noiseHeight += lerp(sampleT, sampleB, blendY) * amplitude;
//
//                amplitude *= scalingBias;
//                frequency *= lacunarity;
//            }
//            perlinNoise2D[y * vertexWidthAmount + x] = noiseHeight;
//        }
//    }
//
//    // normalize noise values to [0, 256]
//    for (int x = 0; x < vertexWidthAmount; x++) {
//        for (int y = 0; y < vertexHeightAmount; y++) {
//            float normalizedHeight = (perlinNoise2D[y * vertexWidthAmount + x]) / (maxPossibleHeight);
//            perlinNoise2D[y * vertexWidthAmount + x] = std::clamp(normalizedHeight, 0.0f, 1.0f) * 1200.0f;
//        }
//    }
//}
//
//bool const Chunk::operator==(const Chunk& other) {
//    return(position == other.position);
//}
//
//float Chunk::lerp(float a, float b, float t) {
//    return a + (b - a) * t;
//}
//
//float Chunk::fade(float t) {
//    return t * t * t * (t * (t * 6 - 15) + 10);
//    //return t* t* (3 - 2 * t);
//}
//
//void Terrain::updateThread() {
//    while (!mapDataThreadInfoQueue.empty()) {
//        std::lock_guard<std::mutex> lock(queueMutex);
//        ChunkThreadData threadInfo = mapDataThreadInfoQueue.front();
//        mapDataThreadInfoQueue.pop();
//        threadInfo.callback(threadInfo.parameter);
//    }
//}
//
//Chunk::Chunk(unsigned int vertexWidthAmount, unsigned int vertexHeightAmount,
//    unsigned int octaveCount, float scalingBias, glm::vec2 position, std::vector<LODInfo> detailLevels) {
//    this->vertexWidthAmount = vertexWidthAmount;
//    this->vertexHeightAmount = vertexHeightAmount;
//    this->octaveCount = octaveCount;
//    this->scalingBias = scalingBias;
//
//    this->noiseSeed2D = nullptr;
//    this->perlinNoise2D = nullptr;
//
//    this->loaded = false;
//    this->position = position;
//    this->previousLodIndex = -1;
//
//    for (int i = 0; i < detailLevels.size(); i++)
//        lodChunks.push_back(LODChunk(detailLevels[i].lod));
//}
//
//
//glm::vec2 Chunk::getPosition() {
//    return position;
//}
//
//unsigned int Chunk::getVertexWidthAmount() {
//    return vertexWidthAmount;
//}
//
//unsigned int Chunk::getVertexHeightAmount() {
//    return vertexHeightAmount;
//}
//
//unsigned int Chunk::getOctaveCount() {
//    return octaveCount;
//}
//
//float Chunk::getScalingBias() {
//    return scalingBias;
//}
//
//std::vector<glm::vec3> Chunk::getVertices() {
//    return vertices;
//}
//
//std::vector<unsigned int> Chunk::getIndices() {
//    return indices;
//}
//
//std::vector<glm::vec3> Chunk::getNormals() {
//    return normals;
//}
//
//float* Chunk::getNoiseSeed2D() {
//    return noiseSeed2D;
//}
//
//void Chunk::setLoaded(bool loaded) {
//    this->loaded = loaded;
//}
//
//bool Chunk::getLoaded() {
//    return loaded;
//}
//
//void Chunk::setNoiseSeed2D(float* noiseSeed2D) {
//    this->noiseSeed2D = noiseSeed2D;
//}
//void Chunk::setPerlinNoise2D(float* perlinNoise2D) {
//    this->perlinNoise2D = perlinNoise2D;
//}
//float* Chunk::getPerlinNoise2D() {
//    return perlinNoise2D;
//}
//
//
//std::vector<glm::vec3>* Chunk::getVerticesPtr() {
//    return &vertices;
//}
//void Chunk::setVertices(std::vector<glm::vec3> vertices) {
//    this->vertices = vertices;
//}
//
//
//std::vector<unsigned int>* Chunk::getIndicesPtr() {
//    return &indices;
//}
//void Chunk::setIndices(std::vector<unsigned int> indices) {
//    this->indices = indices;
//}
//
//std::vector<glm::vec3>* Chunk::getNormalsPtr() {
//    return &normals;
//}
//void Chunk::setNormals(std::vector<glm::vec3> normals) {
//    this->normals = normals;
//}
//
//TerrainVao Chunk::getVAO() {
//    return vao;
//}
//
//void Chunk::setVBO(VBO vbo) {
//    this->vbo = vbo;
//};
//VBO Chunk::getVBO() {
//    return vbo;
//}
//
//IBO Chunk::getIBO() {
//    return ibo;
//}
//
//void Chunk::setVAO(TerrainVao vao) {
//    this->vao = vao;
//}
//void Chunk::setIBO(IBO ibo) {
//    this->ibo = ibo;
//}
//
//int Chunk::getPreviousLodIndex() {
//    return previousLodIndex;
//}
//unsigned int Chunk::getNumStrips() {
//    return numStrips;
//}
//unsigned int Chunk::getNumVertsPerStrip() {
//    return numVertsPerStrip;
//}
//
//void Chunk::setPreviousLodIndex(int previousLodIndex) {
//    this->previousLodIndex = previousLodIndex;
//}
//
//void Chunk::setNumStrips(unsigned int numStrips) {
//    this->numStrips = numStrips;
//}
//void Chunk::setNumVertsPerStrip(unsigned int numVertsPerStrip) {
//    this->numVertsPerStrip = numVertsPerStrip;
//}
//
//LODChunk Chunk::getLodChunk(int elementId) {
//    return lodChunks[elementId];
//};
//
//LODChunk* Chunk::getLodChunkPtr(int elementId) {
//    return &lodChunks[elementId];
//};
//
//std::vector<LODChunk> Chunk::getLodChunks() {
//    return lodChunks;
//}
//
//void Chunk::setPosition(glm::vec2 position) {
//    this->position = position;
//}
//
//LODChunk::LODChunk(int lod) {
//    this->lod = lod;
//    hasMesh = false;
//    hasRequestedMesh = false;
//    this->Mesh = new Chunk();
//}
//
//bool LODChunk::getHasChunk() {
//    return hasMesh;
//}
//bool LODChunk::getHasRequestedChunk() {
//    return hasRequestedMesh;
//}
//int LODChunk::getLOD() {
//    return lod;
//}
//
//void LODChunk::setHasRequestedChunk(bool hasRequestedChunk) {
//    this->hasRequestedMesh = hasRequestedChunk;
//}
//
//Chunk* LODChunk::getChunk() {
//    return Mesh;
//}
//
//unsigned int Terrain::getVertexWidthAmount() {
//    return vertexWidthAmount;
//};
//unsigned int Terrain::getVertexHeightAmount() {
//    return vertexHeightAmount;
//};
//
//std::vector<Chunk*> Terrain::getVisibleChunks() {
//    return visibleChunks;
//};






//
//#include "Terrain/Terrain.h"
//
//Terrain::Terrain(Camera* camera, unsigned int vertexWidthAmount, unsigned int vertexHeightAmount, unsigned int octaveCount, float scalingBias) {
//    this->camera = camera;
//    this->vertexWidthAmount = vertexWidthAmount;
//    this->vertexHeightAmount = vertexHeightAmount;
//    this->octaveCount = octaveCount;
//    this->scalingBias = scalingBias;
//
//    //double distToChunkEdge = sqrt((vertexWidthAmount - 1) * (vertexWidthAmount - 1) + (vertexHeightAmount - 1) * (vertexHeightAmount - 1));
//    double distToChunkEdge = (vertexWidthAmount - 1);
//    //detailLevels = { LODInfo(8, distToChunkEdge * 8) };
//
//    detailLevels = { LODInfo(0, distToChunkEdge * 6), LODInfo(4, distToChunkEdge * 7), LODInfo(8, distToChunkEdge * 8) };
//    //detailLevels = { LODInfo(0, distToChunkEdge * 4), LODInfo(1,distToChunkEdge * 6), LODInfo(2,distToChunkEdge * 8) };
//
//    std::sort(detailLevels.begin(), detailLevels.end(),
//        [](const LODInfo& a, const LODInfo& b) {
//            return a.visibleDstThreshold < b.visibleDstThreshold;
//        });
//
//    this->maxViewDst = detailLevels[detailLevels.size() - 1].visibleDstThreshold;
//    this->chunkSize = vertexWidthAmount - 1;
//    //this->chunksVisibleInViewDst = int(maxViewDst) / chunkSize;
//    this->chunksVisibleInViewDst = static_cast<int>(std::ceil(maxViewDst / chunkSize));
//}
//
//Terrain::Terrain(const Terrain& other) :
//    camera(other.camera),
//    vertexWidthAmount(other.vertexWidthAmount),
//    vertexHeightAmount(other.vertexHeightAmount),
//    octaveCount(other.octaveCount),
//    scalingBias(other.scalingBias),
//    maxViewDst(other.maxViewDst),
//    chunkSize(other.chunkSize),
//    chunksVisibleInViewDst(other.chunksVisibleInViewDst),
//    detailLevels(other.detailLevels)
//{
//    // Initialize empty containers
//    chunks.clear();
//    visibleChunks.clear();
//    mapDataThreadInfoQueue = {};
//
//    // Manually reconstruct chunks
//    for (const auto& pair : other.chunks) {
//        const glm::vec2& pos = pair.first;
//        const Chunk& otherChunk = pair.second;
//
//        // Construct new chunk in place
//        auto [it, inserted] = chunks.emplace(
//            std::piecewise_construct,
//            std::forward_as_tuple(pos),
//            std::forward_as_tuple(
//                otherChunk.getVertexWidthAmount(),
//                otherChunk.getVertexHeightAmount(),
//                otherChunk.getOctaveCount(),
//                otherChunk.getScalingBias(),
//                pos,
//                detailLevels
//            )
//        );
//
//        // Rebuild visibility
//        if (otherChunk.getLoaded()) {
//            visibleChunks.insert(&it->second);
//        }
//    }
//}
//
//Terrain& Terrain::operator=(const Terrain& other) {
//    if (this != &other) {
//        // Copy simple members
//        camera = other.camera;
//        vertexWidthAmount = other.vertexWidthAmount;
//        vertexHeightAmount = other.vertexHeightAmount;
//        octaveCount = other.octaveCount;
//        scalingBias = other.scalingBias;
//        maxViewDst = other.maxViewDst;
//        chunkSize = other.chunkSize;
//        chunksVisibleInViewDst = other.chunksVisibleInViewDst;
//        detailLevels = other.detailLevels;
//
//        // Clear existing chunks (properly clean up OpenGL resources)
//        for (auto& pair : chunks) {
//            pair.second.vao->Delete();
//            pair.second.ibo->Delete();
//            pair.second.vbo->Delete();
//            pair.second.vao.reset();
//            pair.second.ibo.reset();
//            pair.second.vbo.reset();
//        }
//        chunks.clear();
//        visibleChunks.clear();
//
//        // Rebuild chunks from scratch (don't copy)
//        for (const auto& pair : other.chunks) {
//            const glm::vec2& pos = pair.first;
//            const Chunk& otherChunk = pair.second;
//
//            // Create new chunk with same parameters
//            chunks.emplace(
//                std::piecewise_construct,
//                std::forward_as_tuple(pos),
//                std::forward_as_tuple(
//                    otherChunk.getVertexWidthAmount(),
//                    otherChunk.getVertexHeightAmount(),
//                    otherChunk.getOctaveCount(),
//                    otherChunk.getScalingBias(),
//                    pos,
//                    detailLevels
//                )
//            );
//
//            // If the other chunk was visible, add to visible list
//            if (otherChunk.getLoaded()) {
//                visibleChunks.insert(&chunks.at(pos));
//            }
//        }
//
//        // Clear the thread queue (thread operations can't be copied)
//        mapDataThreadInfoQueue = {};
//    }
//    return *this;
//}
//
//void Terrain::updateVisibleChunks() {
//    float camPosX = camera->getCameraPosition().x;
//    float camPosZ = camera->getCameraPosition().z;
//
//    // Use ROUND instead of FLOOR to center chunk detection
//    int currentChunkCoordX = static_cast<int>(std::floor(camPosX / chunkSize));
//    int currentChunkCoordZ = static_cast<int>(std::floor(camPosZ / chunkSize));
//
//    std::unordered_set<glm::vec2> visible;
//
//    std::lock_guard<std::mutex> lock(queueMutex);
//
//    // Original loop structure (-chunksVisibleInViewDst to +chunksVisibleInViewDst)
//    for (int yOffset = -chunksVisibleInViewDst; yOffset <= chunksVisibleInViewDst; yOffset++) {
//        for (int xOffset = -chunksVisibleInViewDst; xOffset <= chunksVisibleInViewDst; xOffset++) {
//            glm::vec2 viewedChunkCoord(currentChunkCoordX + xOffset, currentChunkCoordZ + yOffset);
//
//            visible.insert(viewedChunkCoord);
//
//            if (chunks.find(viewedChunkCoord) == chunks.end()) {
//                chunks.try_emplace(viewedChunkCoord,
//                    Chunk(vertexWidthAmount, vertexHeightAmount, octaveCount,
//                        scalingBias, viewedChunkCoord, detailLevels));
//                chunkDataThreadStart(0, viewedChunkCoord, [this, &chunk, lodIndex, currChunkCoordX, currChunkCoordZ, camPosX, camPosZ](Chunk&) {
//                                            chunk.getLodChunkPtr(lodIndex)->onChunkDataReceived(*chunk.getLodChunk(lodIndex).getChunk());
//                                            updateChunk(chunk, currChunkCoordX, currChunkCoordZ, camPosX, camPosZ);
//                                            });
//            }
//        }
//    }
//
//    //float camPosX = camera->getCameraPosition().x;
//    //float camPosZ = camera->getCameraPosition().z;
//
//    //int currentChunkCoordX = static_cast<int>(std::floor(camPosX / chunkSize));
//    //int currentChunkCoordZ = static_cast<int>(std::floor(camPosZ / chunkSize));
//
//    //std::unordered_set<glm::vec2> visible;
//
//    //std::lock_guard<std::mutex> lock(queueMutex);
//
//    //// Calculate the range based on chunksVisibleInViewDst but check actual distance
//    //for (int yOffset = -chunksVisibleInViewDst; yOffset <= chunksVisibleInViewDst; yOffset++) {
//    //    for (int xOffset = -chunksVisibleInViewDst; xOffset <= chunksVisibleInViewDst; xOffset++) {
//    //        glm::vec2 viewedChunkCoord(currentChunkCoordX + xOffset, currentChunkCoordZ + yOffset);
//
//    //        // Calculate chunk's center in world coordinates
//    //        glm::vec2 chunkCenter = viewedChunkCoord * static_cast<float>(chunkSize) + glm::vec2(chunkSize / 2.0f);
//    //        float dx = chunkCenter.x - camPosX;
//    //        float dz = chunkCenter.y - camPosZ;
//    //        float distanceSq = dx * dx + dz * dz;
//
//    //        // Check if within max view distance
//    //        if (distanceSq <= maxViewDst * maxViewDst) {
//    //            visible.insert(viewedChunkCoord);
//
//    //            if (!chunks.count(viewedChunkCoord)) {
//    //                chunks.try_emplace(viewedChunkCoord,
//    //                    Chunk(vertexWidthAmount, vertexHeightAmount, octaveCount,
//    //                        scalingBias, viewedChunkCoord, detailLevels));
//    //            }
//    //        }
//    //    }
//    //}
//
//    //// Update visibility for ALL existing chunks
//    //for (auto& [coord, chunk] : chunks) {
//    //    updateChunk(chunk, currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
//    //}
//
//    //// NEW: Optimized chunk cleanup
//    //for (auto it = chunks.begin(); it != chunks.end();) {
//    //    if (!visible.count(it->first)) {
//    //        it->second.cleanup();  // Use new cleanup method
//    //        it = chunks.erase(it);
//    //    }
//    //    else {
//    //        updateChunk(it->second, currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
//    //        ++it;
//    //    }
//    //}
//
//    for (auto& [coord, chunk] : chunks) {
//        if (visible.count(coord)) {
//            updateChunk(chunk, currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
//        }
//        else {
//            chunk.cleanup();
//            chunks.erase(coord);
//        }
//    }
//
//}
//
//void Terrain::updateChunk(Chunk& chunk, int currChunkCoordX, int currChunkCoordZ, float camPosX, float camPosZ) {
//    glm::vec2 chunkPos = chunk.getPosition();
//
//    // World position of chunk's edges
//    float xMin = chunkPos.x * chunkSize;
//    float xMax = xMin + chunkSize;
//    float zMin = chunkPos.y * chunkSize;
//    float zMax = zMin + chunkSize;
//
//    // Distance to the NEAREST edge (left/right for X, top/bottom for Z)
//    float dx = std::max(0.0f, std::max(xMin - camPosX, camPosX - xMax));
//    float dz = std::max(0.0f, std::max(zMin - camPosZ, camPosZ - zMax));
//
//    // Euclidean distance to the nearest edge
//    float distanceToNearestEdge = std::sqrt(dx * dx + dz * dz);
//
//    bool visible = (distanceToNearestEdge <= maxViewDst);
//
//
//    if (visible) {
//        // Determine required LOD directly from detailLevels
//        int requiredLOD = 0;
//        for (const auto& level : detailLevels) {
//            if (distanceToNearestEdge > level.visibleDstThreshold) {
//                requiredLOD = level.lod;
//            }
//            else {
//                break;
//            }
//        }
//
//        // Only regenerate if LOD changed
//        if (requiredLOD != chunk.currentLOD) {
//            chunk.currentLOD = requiredLOD;
//            chunk.needsRegeneration = true;
//        }
//
//        // Generate mesh if needed
//        if (chunk.needsRegeneration) {
//            chunk.generateChunkData(requiredLOD);
//            chunk.needsRegeneration = false;
//        }
//    }
//
//    chunk.setLoaded(visible);
//}
//
//
//
//
//void Terrain::updateReceivedChunk(glm::vec2 chunkPos) {
//    float camPosX = camera->getCameraPosition().x;
//    float camPosZ = camera->getCameraPosition().z;
//
//    int currentChunkCoordX = floor(camPosX / chunkSize);
//    int currentChunkCoordZ = floor(camPosZ / chunkSize);
//    updateChunk(chunks[chunkPos], currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
//}
//
//void LODChunk::onChunkDataReceived(Chunk& chunk) {
//    // Ensure we have data to work with
//    if (chunk.getVertices().empty() || chunk.getNormals().empty()) {
//        throw std::runtime_error("Cannot create GPU resources without vertex data");
//    }
//
//    // Create resources
//    chunk.vbo = std::make_unique<VBO>(chunk.getVertices(), chunk.getNormals());
//    chunk.vao = std::make_unique<TerrainVao>(*chunk.vbo);
//
//    if (!chunk.getIndices().empty()) {
//        chunk.vao->Bind();
//        chunk.ibo = std::make_unique<IBO>(chunk.getIndices());
//        chunk.vao->Unbind();
//    }
//
//    if (!hasMesh) {
//        //// Create resources using the chunk's data
//        //Mesh->setVertices(chunk.getVertices());
//        //Mesh->setIndices(chunk.getIndices());
//        //Mesh->setNormals(chunk.getNormals());
//
//        //// Create OpenGL resources
//        //Mesh->vbo = std::make_unique<VBO>(Mesh->getVertices(), Mesh->getNormals());
//        //Mesh->vao = std::make_unique<TerrainVao>(*Mesh->vbo);
//        //Mesh->ibo = std::make_unique<IBO>(Mesh->getIndices());
//
//        hasMesh = true;
//    }
//}
//
//
//void Chunk::updateGPUResources() {
//    if (vao) {
//        vao->Delete();
//        vbo->Delete();
//        ibo->Delete();
//    }
//
//    vbo = std::make_unique<VBO>(vertices, normals);
//    vao = std::make_unique<TerrainVao>(*vbo);
//    ibo = std::make_unique<IBO>(indices);
//
//    // Optional: Clear CPU data after GPU upload
//    vertices.clear();
//    normals.clear();
//    indices.clear();
//    vertices.shrink_to_fit();
//    normals.shrink_to_fit();
//    indices.shrink_to_fit();
//}
//
//void Terrain::chunkDataThreadStart(int lod, glm::vec2 pos, std::function<void(Chunk&)> callbackFunc) {
//    std::thread([this, lod, pos, callbackFunc]() {
//        Chunk tempChunk(vertexWidthAmount, vertexHeightAmount, octaveCount,
//            scalingBias, pos, detailLevels);
//        tempChunk.generateChunkData(lod);
//
//        {
//            std::lock_guard<std::mutex> lock(queueMutex);
//            if (auto it = chunks.find(pos); it != chunks.end()) {
//                it->second.currentLOD = std::move(tempChunk.currentLOD);
//                mapDataThreadInfoQueue.emplace(callbackFunc, pos);
//            }
//        }
//        }).detach();
//}
//
//void Chunk::generateChunkData(int lod) {
//    // Clear old data
//    vertices.clear();
//    normals.clear();
//    indices.clear();
//
//    // Generate noise if needed (shared across LODs)
//    if (this->perlinNoise2D.empty()) {
//        this->GenerateNoiseMap();
//    }
//
//
//    unsigned int meshSimplificationIncrement = (lod == 0) ? 1 : lod * 2;
//    const int verticesPerLine = ((vertexWidthAmount - 1) / meshSimplificationIncrement) + 1;
//
//    // Generate vertices (using noise data)
//    for (unsigned int i = 0; i < vertexWidthAmount; i += meshSimplificationIncrement) {
//        for (unsigned int j = 0; j < vertexHeightAmount; j += meshSimplificationIncrement) {
//            const float y = perlinNoise2D[j * vertexWidthAmount + i];
//            vertices.emplace_back(i, y, j);
//        }
//    }
//
//    // Generate indices
//    for (unsigned int i = 0; i < verticesPerLine - 1; i++) {
//        for (unsigned int j = 0; j < verticesPerLine; j++) {
//            for (unsigned int k = 0; k < 2; k++) {
//                indices.push_back(verticesPerLine * (i + k) + j);
//            }
//        }
//    }
//
//    // Generate normals
//    normals.resize(vertices.size(), glm::vec3(0));
//    for (size_t i = 0; i + 2 < indices.size(); i++) {
//        int index1 = indices[i];
//        int index2 = indices[i + 1];
//        int index3 = indices[i + 2];
//
//        // Calculate the normal for this triangle
//        auto edge1 = vertices[index2] - vertices[index1];
//        auto edge2 = vertices[index3] - vertices[index1];
//        auto normal = glm::cross(edge1, edge2);
//
//        // Ensure the normal faces the correct direction
//        if (normal.y < 0) {
//            normal = -normal;
//        }
//
//        normals[index1] += normal;
//        normals[index2] += normal;
//        normals[index3] += normal;
//    }
//
//    for (size_t i = 0; i < normals.size(); i++) {
//        normals[i] = glm::normalize(normals[i]);
//    }
//
//    updateGPUResources();
//}
//
//
//void Chunk::cleanup() {
//    // CPU data
//    vertices.clear();
//    normals.clear();
//    indices.clear();
//    noiseSeed2D.clear();
//    perlinNoise2D.clear();
//
//    // GPU resources
//    if (vao) vao->Delete();
//    if (ibo) ibo->Delete();
//    if (vbo) vbo->Delete();
//
//    currentLOD = -1;
//}
//
//
//void Chunk::GenerateNoiseMap() {
//    this->perlinNoise2D.resize(vertexWidthAmount * vertexHeightAmount);
//
//    // random values generation
//    std::mt19937 gen(9125123);
//    std::uniform_real_distribution<float> dist(0.0, 1.0);
//    this->noiseSeed2D.resize(vertexWidthAmount * vertexHeightAmount);
//    for (unsigned int i = 0; i < vertexWidthAmount * vertexHeightAmount; i++) {
//        this->noiseSeed2D[i] = dist(gen); // [0.0, 1.0]
//    }
//
//    float chunkScale = 590.0f;
//    float amplitude = 1.0f; // controls the strength of each octave contribution scalingBias - how much amplitude decreases for each octave
//    float frequency = 1.0f; // controls details for each octave
//    float lacunarity = 2.0f; // how much frequency increases for each octave
//
//    float maxPossibleHeight = 0.0f;
//    // calculate maximum possible height for furhter normalization
//    for (int oct = 0; oct < octaveCount; oct++) {
//        maxPossibleHeight += amplitude;
//        amplitude *= scalingBias;
//    }
//
//    // generate Perlin noise
//    for (int x = 0; x < vertexWidthAmount; x++) {
//        for (int y = 0; y < vertexHeightAmount; y++) {
//            float noiseHeight = 0.0f;
//            amplitude = 1.0f;
//            frequency = 1.0f;
//
//            // Iterate through octaves
//            for (int oct = 0; oct < octaveCount; oct++) {
//                // Calculate global sample coordinates
//                float sampleX = (x + position.x * (vertexWidthAmount - 1)) / chunkScale * frequency;
//                float sampleY = (y + position.y * (vertexHeightAmount - 1)) / chunkScale * frequency;
//
//                // interpolation factors
//                float blendX = fade(sampleX - std::floor(sampleX));
//                float blendY = fade(sampleY - std::floor(sampleY));
//
//                auto wrap = [](float value, int size) {
//                    return std::fmod(std::fmod(value, size) + size, size);  // adding size avoids negative values
//                    };
//                // get close to each other points
//                int wrappedX1 = wrap(sampleX, vertexWidthAmount);
//                int wrappedX2 = wrap(sampleX + 1, vertexWidthAmount);
//                int wrappedY1 = wrap(sampleY, vertexHeightAmount);
//                int wrappedY2 = wrap(sampleY + 1, vertexHeightAmount);
//
//                // linear interpolation
//                float sampleT = lerp(noiseSeed2D[wrappedY1 * vertexWidthAmount + wrappedX1],
//                    noiseSeed2D[wrappedY1 * vertexWidthAmount + wrappedX2], blendX);
//                float sampleB = lerp(noiseSeed2D[wrappedY2 * vertexWidthAmount + wrappedX1],
//                    noiseSeed2D[wrappedY2 * vertexWidthAmount + wrappedX2], blendX);
//                noiseHeight += lerp(sampleT, sampleB, blendY) * amplitude;
//
//                amplitude *= scalingBias;
//                frequency *= lacunarity;
//            }
//            perlinNoise2D[y * vertexWidthAmount + x] = noiseHeight;
//        }
//    }
//
//    // normalize noise values to [0, 256]
//    for (int x = 0; x < vertexWidthAmount; x++) {
//        for (int y = 0; y < vertexHeightAmount; y++) {
//            float normalizedHeight = (perlinNoise2D[y * vertexWidthAmount + x]) / (maxPossibleHeight);
//            perlinNoise2D[y * vertexWidthAmount + x] = std::clamp(normalizedHeight, 0.0f, 1.0f) * 1200.0f;
//        }
//    }
//}
//
//bool const Chunk::operator==(const Chunk& other) {
//    return(position == other.position);
//}
//
//float Chunk::lerp(float a, float b, float t) {
//    return a + (b - a) * t;
//}
//
//float Chunk::fade(float t) {
//    return t * t * t * (t * (t * 6 - 15) + 10);
//    //return t* t* (3 - 2 * t);
//}
//
//void Terrain::updateThread() {
//    while (!mapDataThreadInfoQueue.empty()) {
//        std::lock_guard<std::mutex> lock(queueMutex);
//        auto task = mapDataThreadInfoQueue.front();
//        mapDataThreadInfoQueue.pop();
//
//        if (auto it = chunks.find(task.second); it != chunks.end()) {
//            task.first(it->second); // This calls updateGPUResources()
//        }
//    }
//
//    // Clear the queue
//    decltype(mapDataThreadInfoQueue){}.swap(mapDataThreadInfoQueue);
//}
//
//
//Chunk::Chunk(unsigned int vertexWidthAmount, unsigned int vertexHeightAmount,
//    unsigned int octaveCount, float scalingBias, glm::vec2 position,
//    std::vector<LODInfo> detailLevels) :
//    vertexWidthAmount(vertexWidthAmount),
//    vertexHeightAmount(vertexHeightAmount),
//    octaveCount(octaveCount),
//    scalingBias(scalingBias),
//    position(position),
//    loaded(false),
//    previousLodIndex(-1),
//    vbo(nullptr),
//    vao(nullptr),
//    ibo(nullptr)
//{}


//#include "Terrain/Terrain.h"
//
//Terrain::Terrain(Camera* camera, unsigned int vertexWidthAmount, unsigned int vertexHeightAmount, unsigned int octaveCount, float scalingBias) {
//    this->camera = camera;
//    this->vertexWidthAmount = vertexWidthAmount;
//    this->vertexHeightAmount = vertexHeightAmount;
//    this->octaveCount = octaveCount;
//    this->scalingBias = scalingBias;
//
//
//        //double distToChunkEdge = sqrt((vertexWidthAmount - 1) * (vertexWidthAmount - 1) + (vertexHeightAmount - 1) * (vertexHeightAmount - 1));
//        double distToChunkEdge = (vertexWidthAmount - 1);
//    //detailLevels = { LODInfo(8, distToChunkEdge * 8) };
//
//    detailLevels = { LODInfo(0, distToChunkEdge * 6), LODInfo(4, distToChunkEdge * 7), LODInfo(8, distToChunkEdge * 8) };
//    //detailLevels = { LODInfo(0, distToChunkEdge * 4), LODInfo(1,distToChunkEdge * 6), LODInfo(2,distToChunkEdge * 8) };
//
//    std::sort(detailLevels.begin(), detailLevels.end(),
//        [](const LODInfo& a, const LODInfo& b) {
//            return a.visibleDstThreshold < b.visibleDstThreshold;
//        });
//
//    this->maxViewDst = detailLevels[detailLevels.size() - 1].visibleDstThreshold;
//    this->chunkSize = vertexWidthAmount - 1;
//    //this->chunksVisibleInViewDst = int(maxViewDst) / chunkSize;
//    this->chunksVisibleInViewDst = static_cast<int>(std::ceil(maxViewDst / chunkSize));
//}
//
//Terrain::Terrain(const Terrain& other) :
//    camera(other.camera),
//    vertexWidthAmount(other.vertexWidthAmount),
//    vertexHeightAmount(other.vertexHeightAmount),
//    octaveCount(other.octaveCount),
//    scalingBias(other.scalingBias),
//    maxViewDst(other.maxViewDst),
//    chunkSize(other.chunkSize),
//    chunksVisibleInViewDst(other.chunksVisibleInViewDst),
//    detailLevels(other.detailLevels)
//{
//    // Initialize empty containers
//    chunks.clear();
//    visibleChunks.clear();
//    mapDataThreadInfoQueue = {};
//
//        // Manually reconstruct chunks
//        for (const auto& pair : other.chunks) {
//            const glm::vec2& pos = pair.first;
//            const Chunk& otherChunk = pair.second;
//
//            // Construct new chunk in place
//            auto [it, inserted] = chunks.emplace(
//                std::piecewise_construct,
//                std::forward_as_tuple(pos),
//                std::forward_as_tuple(
//                    otherChunk.getVertexWidthAmount(),
//                    otherChunk.getVertexHeightAmount(),
//                    otherChunk.getOctaveCount(),
//                    otherChunk.getScalingBias(),
//                    pos,
//                    detailLevels
//                )
//            );
//
//            // Rebuild visibility
//            if (otherChunk.getLoaded()) {
//                visibleChunks.insert(&it->second);
//            }
//        }
//}
//
//Terrain& Terrain::operator=(const Terrain& other) {
//    if (this != &other) {
//        // Copy simple members
//        camera = other.camera;
//        vertexWidthAmount = other.vertexWidthAmount;
//        vertexHeightAmount = other.vertexHeightAmount;
//        octaveCount = other.octaveCount;
//        scalingBias = other.scalingBias;
//        maxViewDst = other.maxViewDst;
//        chunkSize = other.chunkSize;
//        chunksVisibleInViewDst = other.chunksVisibleInViewDst;
//        detailLevels = other.detailLevels;
//
//            // Clear existing chunks (properly clean up OpenGL resources)
//            for (auto& pair : chunks) {
//                pair.second.vao->Delete();
//                pair.second.ibo->Delete();
//                pair.second.vbo->Delete();
//                pair.second.vao.reset();
//                pair.second.ibo.reset();
//                pair.second.vbo.reset();
//            }
//        chunks.clear();
//        visibleChunks.clear();
//
//        // Rebuild chunks from scratch (don't copy)
//        for (const auto& pair : other.chunks) {
//            const glm::vec2& pos = pair.first;
//            const Chunk& otherChunk = pair.second;
//
//            // Create new chunk with same parameters
//            chunks.emplace(
//                std::piecewise_construct,
//                std::forward_as_tuple(pos),
//                std::forward_as_tuple(
//                    otherChunk.getVertexWidthAmount(),
//                    otherChunk.getVertexHeightAmount(),
//                    otherChunk.getOctaveCount(),
//                    otherChunk.getScalingBias(),
//                    pos,
//                    detailLevels
//                )
//            );
//
//            // If the other chunk was visible, add to visible list
//            if (otherChunk.getLoaded()) {
//                visibleChunks.insert(&chunks.at(pos));
//            }
//        }
//
//        // Clear the thread queue (thread operations can't be copied)
//        mapDataThreadInfoQueue = {};
//    }
//    return *this;
//
//
//}
//
//void Terrain::updateVisibleChunks() {
//    float camPosX = camera->getCameraPosition().x;
//    float camPosZ = camera->getCameraPosition().z;
//
//        // Use ROUND instead of FLOOR to center chunk detection
//        int currentChunkCoordX = static_cast<int>(std::floor(camPosX / chunkSize));
//    int currentChunkCoordZ = static_cast<int>(std::floor(camPosZ / chunkSize));
//
//    std::unordered_set<glm::vec2> visible;
//
//    std::lock_guard<std::mutex> lock(queueMutex);
//
//    // Original loop structure (-chunksVisibleInViewDst to +chunksVisibleInViewDst)
//    for (int yOffset = -chunksVisibleInViewDst; yOffset <= chunksVisibleInViewDst; yOffset++) {
//        for (int xOffset = -chunksVisibleInViewDst; xOffset <= chunksVisibleInViewDst; xOffset++) {
//            glm::vec2 viewedChunkCoord(currentChunkCoordX + xOffset, currentChunkCoordZ + yOffset);
//
//            visible.insert(viewedChunkCoord);
//
//            if (chunks.find(viewedChunkCoord) == chunks.end()) {
//                chunks.try_emplace(viewedChunkCoord,
//                    Chunk(vertexWidthAmount, vertexHeightAmount, octaveCount,
//                        scalingBias, viewedChunkCoord, detailLevels));
//            }
//        }
//    }
//
//    //float camPosX = camera->getCameraPosition().x;
//    //float camPosZ = camera->getCameraPosition().z;
//
//    //int currentChunkCoordX = static_cast<int>(std::floor(camPosX / chunkSize));
//    //int currentChunkCoordZ = static_cast<int>(std::floor(camPosZ / chunkSize));
//
//    //std::unordered_set<glm::vec2> visible;
//
//    //std::lock_guard<std::mutex> lock(queueMutex);
//
//    //// Calculate the range based on chunksVisibleInViewDst but check actual distance
//    //for (int yOffset = -chunksVisibleInViewDst; yOffset <= chunksVisibleInViewDst; yOffset++) {
//    //    for (int xOffset = -chunksVisibleInViewDst; xOffset <= chunksVisibleInViewDst; xOffset++) {
//    //        glm::vec2 viewedChunkCoord(currentChunkCoordX + xOffset, currentChunkCoordZ + yOffset);
//
//    //        // Calculate chunk's center in world coordinates
//    //        glm::vec2 chunkCenter = viewedChunkCoord * static_cast<float>(chunkSize) + glm::vec2(chunkSize / 2.0f);
//    //        float dx = chunkCenter.x - camPosX;
//    //        float dz = chunkCenter.y - camPosZ;
//    //        float distanceSq = dx * dx + dz * dz;
//
//    //        // Check if within max view distance
//    //        if (distanceSq <= maxViewDst * maxViewDst) {
//    //            visible.insert(viewedChunkCoord);
//
//    //            if (!chunks.count(viewedChunkCoord)) {
//    //                chunks.try_emplace(viewedChunkCoord,
//    //                    Chunk(vertexWidthAmount, vertexHeightAmount, octaveCount,
//    //                        scalingBias, viewedChunkCoord, detailLevels));
//    //            }
//    //        }
//    //    }
//    //}
//
//    // Update visibility for ALL existing chunks
//    for (auto& [coord, chunk] : chunks) {
//        updateChunk(chunk, currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
//    }
//
//    for (auto it = chunks.begin(); it != chunks.end();) {
//        if (!visible.count(it->first)) {
//            // 1) Delete GPU buffers
//            if (it->second.vao) it->second.vao->Delete();
//            if (it->second.ibo) it->second.ibo->Delete();
//            if (it->second.vbo) it->second.vbo->Delete();
//            it->second.vao.reset();
//            it->second.ibo.reset();
//            it->second.vbo.reset();
//
//            // 2) Clear all LOD‐chunk CPU data
//            for (auto& lodW : it->second.getLodChunks()) {
//                if (Chunk* lodChunk = lodW.getChunk()) {
//                    // swap with empty to free both size and capacity
//                    std::vector<glm::vec3>().swap(lodChunk->getVerticesRef());
//                    std::vector<glm::vec3>().swap(lodChunk->getNormalsRef());
//                    std::vector<unsigned int>().swap(lodChunk->getIndicesRef());
//                }
//            }
//
//            // 3) Clear master‐Chunk CPU data
//            it->second.vertices.clear();    it->second.vertices.shrink_to_fit();
//            it->second.normals.clear();     it->second.normals.shrink_to_fit();
//            it->second.indices.clear();     it->second.indices.shrink_to_fit();
//            it->second.noiseSeed2D.clear(); it->second.noiseSeed2D.shrink_to_fit();
//            it->second.perlinNoise2D.clear(); it->second.perlinNoise2D.shrink_to_fit();
//            it->second.lodChunks.clear();   it->second.lodChunks.shrink_to_fit();
//
//            it->second.setLoaded(false);
//            it->second.setPreviousLodIndex(-1);
//            it->second.setNumStrips(0);
//            it->second.setNumVertsPerStrip(0);
//
//            // 4) Finally erase the chunk
//            it = chunks.erase(it);
//        }
//        else {
//            ++it;
//        }
//    }
//
//}
//
//void Terrain::updateChunk(Chunk& chunk, int currChunkCoordX, int currChunkCoordZ, float camPosX, float camPosZ) {
//    glm::vec2 chunkPos = chunk.getPosition();
//
//        // World position of chunk's edges
//        float xMin = chunkPos.x * chunkSize;
//    float xMax = xMin + chunkSize;
//    float zMin = chunkPos.y * chunkSize;
//    float zMax = zMin + chunkSize;
//
//    // Distance to the NEAREST edge (left/right for X, top/bottom for Z)
//    float dx = std::max(0.0f, std::max(xMin - camPosX, camPosX - xMax));
//    float dz = std::max(0.0f, std::max(zMin - camPosZ, camPosZ - zMax));
//
//    // Euclidean distance to the nearest edge
//    float distanceToNearestEdge = std::sqrt(dx * dx + dz * dz);
//
//    bool visible = (distanceToNearestEdge <= maxViewDst);
//
//
//    if (visible) {
//        int lodIndex = 0;
//        for (int i = 0; i < detailLevels.size() - 1; i++) {
//            if (distanceToNearestEdge > detailLevels[i].visibleDstThreshold) {
//                lodIndex = i + 1;
//            }
//            else {
//                break;
//            }
//        }
//        // if chunk should be changed
//        if (lodIndex != chunk.getPreviousLodIndex()) {
//            // if chunk with this lod has been loaded
//            if (lodIndex != chunk.getPreviousLodIndex()) {
//                // if chunk with this lod has been loaded
//                if (chunk.getLodChunk(lodIndex).getHasChunk()) {
//                    chunk.setPreviousLodIndex(lodIndex);
//
//                    if (chunk.vao)
//                        chunk.vao->Delete();
//                    if (chunk.ibo)
//                        chunk.ibo->Delete();
//                    if (chunk.vbo)
//                        chunk.vbo->Delete();
//
//                    chunk.vao.reset();
//                    chunk.vbo.reset();
//                    chunk.vbo.reset();
//
//                    auto& lodChunk = *chunk.getLodChunk(lodIndex).getChunk();
//                    chunk.vbo = std::make_unique<VBO>(lodChunk.getVertices(), lodChunk.getNormals());
//                    chunk.vao = std::make_unique<TerrainVao>(*chunk.vbo);
//                    chunk.vao->Bind();
//                    chunk.ibo = std::make_unique<IBO>(lodChunk.getIndices());
//                    chunk.vao->Unbind();
//                    chunk.setNumStrips(lodChunk.getNumStrips());
//                    chunk.setNumVertsPerStrip(lodChunk.getNumVertsPerStrip());
//                }
//                // if chunk with this lod hasn't been created
//                else if (!chunk.getLodChunk(lodIndex).getHasRequestedChunk()) {
//                    chunk.getLodChunkPtr(lodIndex)->setHasRequestedChunk(true);
//
//                    chunkDataThreadStart(lodIndex, chunkPos, [this, &chunk, lodIndex, currChunkCoordX, currChunkCoordZ, camPosX, camPosZ](Chunk&) {
//                        chunk.getLodChunkPtr(lodIndex)->onChunkDataReceived(*chunk.getLodChunk(lodIndex).getChunk());
//                        updateChunk(chunk, currChunkCoordX, currChunkCoordZ, camPosX, camPosZ);
//                        });
//                }
//            }
//        }
//    }
//
//    if (chunk.getLoaded() != visible) {
//        if (visible) {
//            visibleChunks.insert(&chunk);
//        }
//        else {
//            visibleChunks.erase(&chunk);
//        }
//    }
//
//    chunk.setLoaded(visible);
//
//
//}
//
//void Terrain::updateReceivedChunk(glm::vec2 chunkPos) {
//    float camPosX = camera->getCameraPosition().x;
//    float camPosZ = camera->getCameraPosition().z;
//
//
//        int currentChunkCoordX = floor(camPosX / chunkSize);
//    int currentChunkCoordZ = floor(camPosZ / chunkSize);
//    updateChunk(chunks[chunkPos], currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
//
//}
//
//void LODChunk::onChunkDataReceived(Chunk& chunk) {
//    // Ensure we have data to work with
//    if (chunk.getVertices().empty() || chunk.getNormals().empty()) {
//        throw std::runtime_error("Cannot create GPU resources without vertex data");
//    }
//
//        // Create resources
//        chunk.vbo = std::make_unique<VBO>(chunk.getVertices(), chunk.getNormals());
//    chunk.vao = std::make_unique<TerrainVao>(*chunk.vbo);
//
//    if (!chunk.getIndices().empty()) {
//        chunk.vao->Bind();
//        chunk.ibo = std::make_unique<IBO>(chunk.getIndices());
//        chunk.vao->Unbind();
//    }
//
//    if (!hasMesh) {
//        //// Create resources using the chunk's data
//        //Mesh->setVertices(chunk.getVertices());
//        //Mesh->setIndices(chunk.getIndices());
//        //Mesh->setNormals(chunk.getNormals());
//
//        //// Create OpenGL resources
//        //Mesh->vbo = std::make_unique<VBO>(Mesh->getVertices(), Mesh->getNormals());
//        //Mesh->vao = std::make_unique<TerrainVao>(*Mesh->vbo);
//        //Mesh->ibo = std::make_unique<IBO>(Mesh->getIndices());
//
//        hasMesh = true;
//    }
//
//}
//
//void Terrain::chunkDataThreadStart(int lod, glm::vec2 pos, std::function<void(Chunk&)> callbackFunc) {
//    std::thread([this, lod, pos, callbackFunc]() {
//        Chunk tempChunk(vertexWidthAmount, vertexHeightAmount, octaveCount,
//            scalingBias, pos, detailLevels);
//        tempChunk.generateChunkData(detailLevels[lod].lod);
//
//        {
//            std::lock_guard<std::mutex> lock(queueMutex);
//            auto& lodWrapper = chunks[pos].getLodChunk(lod);
//            Chunk* dst = lodWrapper.getChunk();
//
//            // Use move semantics to transfer ownership
//            dst->getVerticesRef() = std::move(tempChunk.getVerticesRef());
//            dst->getNormalsRef() = std::move(tempChunk.getNormalsRef());
//            dst->getIndicesRef() = std::move(tempChunk.getIndicesRef());
//            dst->setNumStrips(tempChunk.getNumStrips());
//            dst->setNumVertsPerStrip(tempChunk.getNumVertsPerStrip());
//            lodWrapper.needsGPUUpdate = true;
//
//            //mapDataThreadInfoQueue.push(std::make_pair(callbackFunc, pos));
//            mapDataThreadInfoQueue.emplace(std::move(callbackFunc), pos);
//        }
//        }).detach();
//
//
//}
//
//void Chunk::generateChunkData(int lod) {
//    if (perlinNoise2D.empty())
//        //createPerlinNoise2D();
//        GenerateNoiseMap();
//
//
//        unsigned int meshSimplificationIncrement = (lod == 0) ? 1 : lod * 2;
//    int verticesPerLine = ((vertexWidthAmount - 1) / meshSimplificationIncrement) + 1;
//
//    numStrips = (vertexHeightAmount - 1) / meshSimplificationIncrement;
//    numVertsPerStrip = verticesPerLine * 2;  // each strip consists of squares, each square is 2 triangles (2 lines)
//
//    // Generate vertices
//    for (unsigned int i = 0; i < vertexWidthAmount; i += meshSimplificationIncrement) {
//        for (unsigned int j = 0; j < vertexHeightAmount; j += meshSimplificationIncrement) {
//            float y = (float)((perlinNoise2D[j * vertexWidthAmount + i]));
//            vertices.push_back(glm::vec3(i, y, j));
//        }
//    }
//
//    for (unsigned int i = 0; i < verticesPerLine - 1; i++)     // for each row (strip)
//        for (unsigned int j = 0; j < verticesPerLine; j++)    // for each column
//            for (unsigned int k = 0; k < 2; k++)            // for each side of the strip (leftand right primitives in square)
//                indices.push_back(verticesPerLine * (i + k) + j);   // top left -> top right -> bottom left -> bottom right
//
//
//    normals.resize(vertices.size(), glm::vec3(0));
//    for (size_t i = 0; i + 2 < indices.size(); i++) {
//        int index1 = indices[i];
//        int index2 = indices[i + 1];
//        int index3 = indices[i + 2];
//
//        // Calculate the normal for this triangle
//        auto edge1 = vertices[index2] - vertices[index1];
//        auto edge2 = vertices[index3] - vertices[index1];
//        auto normal = glm::cross(edge1, edge2);
//
//        // Ensure the normal faces the correct direction
//        if (normal.y < 0) {
//            normal = -normal;
//        }
//
//        normals[index1] += normal;
//        normals[index2] += normal;
//        normals[index3] += normal;
//    }
//
//    for (size_t i = 0; i < normals.size(); i++) {
//        normals[i] = glm::normalize(normals[i]);
//    }
//
//    perlinNoise2D.clear();
//    perlinNoise2D.shrink_to_fit();
//    noiseSeed2D.clear();
//    noiseSeed2D.shrink_to_fit();
//
//
//}
//
//void Chunk::GenerateNoiseMap() {
//    this->perlinNoise2D.resize(vertexWidthAmount * vertexHeightAmount);
//
//
//        // random values generation
//        std::mt19937 gen(9125123);
//    std::uniform_real_distribution<float> dist(0.0, 1.0);
//    this->noiseSeed2D.resize(vertexWidthAmount * vertexHeightAmount);
//    for (unsigned int i = 0; i < vertexWidthAmount * vertexHeightAmount; i++) {
//        this->noiseSeed2D[i] = dist(gen); // [0.0, 1.0]
//    }
//
//    float chunkScale = 590.0f;
//    float amplitude = 1.0f; // controls the strength of each octave contribution scalingBias - how much amplitude decreases for each octave
//    float frequency = 1.0f; // controls details for each octave
//    float lacunarity = 2.0f; // how much frequency increases for each octave
//
//    float maxPossibleHeight = 0.0f;
//    // calculate maximum possible height for furhter normalization
//    for (int oct = 0; oct < octaveCount; oct++) {
//        maxPossibleHeight += amplitude;
//        amplitude *= scalingBias;
//    }
//
//    // generate Perlin noise
//    for (int x = 0; x < vertexWidthAmount; x++) {
//        for (int y = 0; y < vertexHeightAmount; y++) {
//            float noiseHeight = 0.0f;
//            amplitude = 1.0f;
//            frequency = 1.0f;
//
//            // Iterate through octaves
//            for (int oct = 0; oct < octaveCount; oct++) {
//                // Calculate global sample coordinates
//                float sampleX = (x + position.x * (vertexWidthAmount - 1)) / chunkScale * frequency;
//                float sampleY = (y + position.y * (vertexHeightAmount - 1)) / chunkScale * frequency;
//
//                // interpolation factors
//                float blendX = fade(sampleX - std::floor(sampleX));
//                float blendY = fade(sampleY - std::floor(sampleY));
//
//                auto wrap = [](float value, int size) {
//                    return std::fmod(std::fmod(value, size) + size, size);  // adding size avoids negative values
//                    };
//                // get close to each other points
//                int wrappedX1 = wrap(sampleX, vertexWidthAmount);
//                int wrappedX2 = wrap(sampleX + 1, vertexWidthAmount);
//                int wrappedY1 = wrap(sampleY, vertexHeightAmount);
//                int wrappedY2 = wrap(sampleY + 1, vertexHeightAmount);
//
//                // linear interpolation
//                float sampleT = lerp(noiseSeed2D[wrappedY1 * vertexWidthAmount + wrappedX1],
//                    noiseSeed2D[wrappedY1 * vertexWidthAmount + wrappedX2], blendX);
//                float sampleB = lerp(noiseSeed2D[wrappedY2 * vertexWidthAmount + wrappedX1],
//                    noiseSeed2D[wrappedY2 * vertexWidthAmount + wrappedX2], blendX);
//                noiseHeight += lerp(sampleT, sampleB, blendY) * amplitude;
//
//                amplitude *= scalingBias;
//                frequency *= lacunarity;
//            }
//            perlinNoise2D[y * vertexWidthAmount + x] = noiseHeight;
//        }
//    }
//
//    // normalize noise values to [0, 256]
//    for (int x = 0; x < vertexWidthAmount; x++) {
//        for (int y = 0; y < vertexHeightAmount; y++) {
//            float normalizedHeight = (perlinNoise2D[y * vertexWidthAmount + x]) / (maxPossibleHeight);
//            perlinNoise2D[y * vertexWidthAmount + x] = std::clamp(normalizedHeight, 0.0f, 1.0f) * 1200.0f;
//        }
//    }
//
//
//}
//
//bool const Chunk::operator==(const Chunk& other) {
//    return(position == other.position);
//}
//
//float Chunk::lerp(float a, float b, float t) {
//    return a + (b - a) * t;
//}
//
//float Chunk::fade(float t) {
//    return t * t * t * (t * (t * 6 - 15) + 10);
//    //return t\* t\* (3 - 2 \* t);
//}
//
//void Terrain::updateThread() {
//    while (!mapDataThreadInfoQueue.empty()) {
//        std::lock_guard<std::mutex>lock(queueMutex);
//        auto task = mapDataThreadInfoQueue.front();
//        mapDataThreadInfoQueue.pop();
//
//
//            auto it = chunks.find(task.second);
//        if (it != chunks.end()) {
//            Chunk& chunk = it->second;
//
//            //// Process all LOD chunks that need GPU updates
//            //for (auto& lodChunkWrapper : chunk.getLodChunks()) {
//            //    if (lodChunkWrapper.needsGPUUpdate && lodChunkWrapper.getChunk()) {
//            //        // Create OpenGL resources in main thread
//            //        auto& lodChunk = *lodChunkWrapper.getChunk();
//            //        lodChunk.vbo = std::make_unique<VBO>(lodChunk.getVertices(), lodChunk.getNormals());
//            //        lodChunk.vao = std::make_unique<TerrainVao>(*lodChunk.vbo);
//            //        lodChunk.vao->Bind();
//            //        lodChunk.ibo = std::make_unique<IBO>(lodChunk.getIndices());
//            //        lodChunk.vao->Unbind();
//            //        lodChunkWrapper.needsGPUUpdate = false;
//            //    }
//            //}
//
//            task.first(chunk);
//
//            for (auto& lodW : chunk.getLodChunks()) {
//                if (!lodW.needsGPUUpdate && lodW.getChunk()) {
//                    auto& c = *lodW.getChunk();
//                    // core mesh data
//                    c.getVerticesPtr()->clear();    c.getVerticesPtr()->shrink_to_fit();
//                    c.getNormalsPtr()->clear();     c.getNormalsPtr()->shrink_to_fit();
//                    c.getIndicesPtr()->clear();     c.getIndicesPtr()->shrink_to_fit();
//                    // the noise maps
//                    c.noiseSeed2D.clear();          c.noiseSeed2D.shrink_to_fit();
//                    c.perlinNoise2D.clear();        c.perlinNoise2D.shrink_to_fit();
//                    // any nested LOD wrappers
//                    c.lodChunks.clear();            c.lodChunks.shrink_to_fit();
//                }
//            }
//
//            //for (auto& lodW : chunk.getLodChunks()) {
//            //    if (auto lodChunk = lodW.getChunk()) {
//            //        lodChunk->vertices.clear();
//            //        lodChunk->vertices.shrink_to_fit();
//            //        lodChunk->normals.clear();
//            //        lodChunk->normals.shrink_to_fit();
//            //        lodChunk->indices.clear();
//            //        lodChunk->indices.shrink_to_fit();
//            //        lodChunk->noiseSeed2D.clear();
//            //        lodChunk->perlinNoise2D.clear();
//            //    }
//            //}
//
//            chunk.getVerticesPtr()->clear();    chunk.getVerticesPtr()->shrink_to_fit();
//            chunk.getNormalsPtr()->clear();     chunk.getNormalsPtr()->shrink_to_fit();
//            chunk.getIndicesPtr()->clear();     chunk.getIndicesPtr()->shrink_to_fit();
//            // the noise maps
//            chunk.noiseSeed2D.clear();          chunk.noiseSeed2D.shrink_to_fit();
//            chunk.perlinNoise2D.clear();        chunk.perlinNoise2D.shrink_to_fit();
//            // any nested LOD wrappers
//            //chunk.lodChunks.clear();            chunk.lodChunks.shrink_to_fit();
//        }
//    }
//
//    std::queue<decltype(mapDataThreadInfoQueue)::value_type> empty;
//    std::swap(mapDataThreadInfoQueue, empty);
//
//
//}
//
//Chunk::Chunk(unsigned int vertexWidthAmount, unsigned int vertexHeightAmount,
//    unsigned int octaveCount, float scalingBias, glm::vec2 position,
//    std::vector<LODInfo> detailLevels) :
//    vertexWidthAmount(vertexWidthAmount),
//    vertexHeightAmount(vertexHeightAmount),
//    octaveCount(octaveCount),
//    scalingBias(scalingBias),
//    position(position),
//    loaded(false),
//    previousLodIndex(-1),
//    vbo(nullptr),
//    vao(nullptr),
//    ibo(nullptr)
//{
//    // Initialize LOD chunks
//    for (const auto& detail : detailLevels) {
//        lodChunks.emplace_back(LODChunk(detail.lod));
//
//   
//            // Initialize the Mesh for each LOD chunk
//            auto& lod_chunk = lodChunks.back();
//        lod_chunk.getChunk()->vertexWidthAmount = this->vertexWidthAmount;
//        lod_chunk.getChunk()->vertexHeightAmount = this->vertexHeightAmount;
//        lod_chunk.getChunk()->octaveCount = this->octaveCount;
//        lod_chunk.getChunk()->scalingBias = this->scalingBias;
//        lod_chunk.getChunk()->position = this->position;
//    }
//
//}




//#include "Terrain/Terrain.h"
//
//Terrain::Terrain(Camera* camera, unsigned int vertexWidthAmount, unsigned int vertexHeightAmount, unsigned int octaveCount, float scalingBias) {
//    this->camera = camera;
//    this->vertexWidthAmount = vertexWidthAmount;
//    this->vertexHeightAmount = vertexHeightAmount;
//    this->octaveCount = octaveCount;
//    this->scalingBias = scalingBias;
//
//
//    //double distToChunkEdge = sqrt((vertexWidthAmount - 1) * (vertexWidthAmount - 1) + (vertexHeightAmount - 1) * (vertexHeightAmount - 1));
//    double distToChunkEdge = (vertexWidthAmount - 1);
//    //detailLevels = { LODInfo(8, distToChunkEdge * 8) };
//
//    detailLevels = { LODInfo(0, distToChunkEdge * 6), LODInfo(4, distToChunkEdge * 7), LODInfo(8, distToChunkEdge * 8) };
//    //detailLevels = { LODInfo(0, distToChunkEdge * 4), LODInfo(1,distToChunkEdge * 6), LODInfo(2,distToChunkEdge * 8) };
//
//    std::sort(detailLevels.begin(), detailLevels.end(),
//        [](const LODInfo& a, const LODInfo& b) {
//            return a.visibleDstThreshold < b.visibleDstThreshold;
//        });
//
//    this->maxViewDst = detailLevels[detailLevels.size() - 1].visibleDstThreshold;
//    this->chunkSize = vertexWidthAmount - 1;
//    //this->chunksVisibleInViewDst = int(maxViewDst) / chunkSize;
//    this->chunksVisibleInViewDst = static_cast<int>(std::ceil(maxViewDst / chunkSize));
//}
//
//Terrain::Terrain(const Terrain& other) :
//    camera(other.camera),
//    vertexWidthAmount(other.vertexWidthAmount),
//    vertexHeightAmount(other.vertexHeightAmount),
//    octaveCount(other.octaveCount),
//    scalingBias(other.scalingBias),
//    maxViewDst(other.maxViewDst),
//    chunkSize(other.chunkSize),
//    chunksVisibleInViewDst(other.chunksVisibleInViewDst),
//    detailLevels(other.detailLevels)
//{
//    // Initialize empty containers
//    chunks.clear();
//    visibleChunks.clear();
//    mapDataThreadInfoQueue = {};
//
//    // Manually reconstruct chunks
//    for (const auto& pair : other.chunks) {
//        const glm::vec2& pos = pair.first;
//        const Chunk& otherChunk = pair.second;
//
//        // Construct new chunk in place
//        auto [it, inserted] = chunks.emplace(
//            std::piecewise_construct,
//            std::forward_as_tuple(pos),
//            std::forward_as_tuple(
//                otherChunk.getVertexWidthAmount(),
//                otherChunk.getVertexHeightAmount(),
//                otherChunk.getOctaveCount(),
//                otherChunk.getScalingBias(),
//                pos,
//                detailLevels
//            )
//        );
//
//        // Rebuild visibility
//        if (otherChunk.getLoaded()) {
//            visibleChunks.insert(&it->second);
//        }
//    }
//}
//
//Terrain& Terrain::operator=(const Terrain& other) {
//    if (this != &other) {
//        // Copy simple members
//        camera = other.camera;
//        vertexWidthAmount = other.vertexWidthAmount;
//        vertexHeightAmount = other.vertexHeightAmount;
//        octaveCount = other.octaveCount;
//        scalingBias = other.scalingBias;
//        maxViewDst = other.maxViewDst;
//        chunkSize = other.chunkSize;
//        chunksVisibleInViewDst = other.chunksVisibleInViewDst;
//        detailLevels = other.detailLevels;
//
//        // Clear existing chunks (properly clean up OpenGL resources)
//        for (auto& pair : chunks) {
//            pair.second.vao->Delete();
//            pair.second.ibo->Delete();
//            pair.second.vbo->Delete();
//            pair.second.vao.reset();
//            pair.second.ibo.reset();
//            pair.second.vbo.reset();
//        }
//        chunks.clear();
//        visibleChunks.clear();
//
//        // Rebuild chunks from scratch (don't copy)
//        for (const auto& pair : other.chunks) {
//            const glm::vec2& pos = pair.first;
//            const Chunk& otherChunk = pair.second;
//
//            // Create new chunk with same parameters
//            chunks.emplace(
//                std::piecewise_construct,
//                std::forward_as_tuple(pos),
//                std::forward_as_tuple(
//                    otherChunk.getVertexWidthAmount(),
//                    otherChunk.getVertexHeightAmount(),
//                    otherChunk.getOctaveCount(),
//                    otherChunk.getScalingBias(),
//                    pos,
//                    detailLevels
//                )
//            );
//
//            // If the other chunk was visible, add to visible list
//            if (otherChunk.getLoaded()) {
//                visibleChunks.insert(&chunks.at(pos));
//            }
//        }
//
//        // Clear the thread queue (thread operations can't be copied)
//        mapDataThreadInfoQueue = {};
//    }
//    return *this;
//
//
//}
//
//void Terrain::updateVisibleChunks() {
//    float camPosX = camera->getCameraPosition().x;
//    float camPosZ = camera->getCameraPosition().z;
//
//    // Use ROUND instead of FLOOR to center chunk detection
//    int currentChunkCoordX = static_cast<int>(std::floor(camPosX / chunkSize));
//    int currentChunkCoordZ = static_cast<int>(std::floor(camPosZ / chunkSize));
//
//    std::unordered_set<glm::vec2> visible;
//
//    std::lock_guard<std::mutex> lock(queueMutex);
//
//    // Original loop structure (-chunksVisibleInViewDst to +chunksVisibleInViewDst)
//    for (int yOffset = -chunksVisibleInViewDst; yOffset <= chunksVisibleInViewDst; yOffset++) {
//        for (int xOffset = -chunksVisibleInViewDst; xOffset <= chunksVisibleInViewDst; xOffset++) {
//            glm::vec2 viewedChunkCoord(currentChunkCoordX + xOffset, currentChunkCoordZ + yOffset);
//
//            visible.insert(viewedChunkCoord);
//
//            if (chunks.find(viewedChunkCoord) == chunks.end()) {
//                chunks.try_emplace(viewedChunkCoord,
//                    Chunk(vertexWidthAmount, vertexHeightAmount, octaveCount,
//                        scalingBias, viewedChunkCoord, detailLevels));
//            }
//        }
//    }
//
//    // Update visibility for ALL existing chunks
//    for (auto& [coord, chunk] : chunks) {
//        updateChunk(chunk, currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
//    }
//
//    for (auto it = chunks.begin(); it != chunks.end();) {
//        if (!visible.count(it->first)) {
//            // 1) Delete GPU buffers
//            if (it->second.vao) it->second.vao->Delete();
//            if (it->second.ibo) it->second.ibo->Delete();
//            if (it->second.vbo) it->second.vbo->Delete();
//            it->second.vao.reset();
//            it->second.ibo.reset();
//            it->second.vbo.reset();
//
//            // 3) Clear master‐Chunk CPU data
//            it->second.vertices.clear();    it->second.vertices.shrink_to_fit();
//            it->second.normals.clear();     it->second.normals.shrink_to_fit();
//            it->second.indices.clear();     it->second.indices.shrink_to_fit();
//            it->second.noiseSeed2D.clear(); it->second.noiseSeed2D.shrink_to_fit();
//            it->second.perlinNoise2D.clear(); it->second.perlinNoise2D.shrink_to_fit();
//            //it->second.lodChunks.clear();   it->second.lodChunks.shrink_to_fit();
//
//            it->second.setLoaded(false);
//            it->second.setPreviousLodIndex(-1);
//            it->second.setNumStrips(0);
//            it->second.setNumVertsPerStrip(0);
//
//            // 4) Finally erase the chunk
//            it = chunks.erase(it);
//        }
//        else {
//            ++it;
//        }
//    }
//
//}
//
//void Terrain::updateChunk(Chunk& chunk, int currChunkCoordX, int currChunkCoordZ, float camPosX, float camPosZ) {
//    glm::vec2 chunkPos = chunk.getPosition();
//
//    // World position of chunk's edges
//    float xMin = chunkPos.x * chunkSize;
//    float xMax = xMin + chunkSize;
//    float zMin = chunkPos.y * chunkSize;
//    float zMax = zMin + chunkSize;
//
//    // Distance calculations (keep your existing code)
//    float dx = std::max(0.0f, std::max(xMin - camPosX, camPosX - xMax));
//    float dz = std::max(0.0f, std::max(zMin - camPosZ, camPosZ - zMax));
//    float distanceToNearestEdge = std::sqrt(dx * dx + dz * dz);
//
//    bool visible = (distanceToNearestEdge <= maxViewDst);
//
//    if (visible) {
//        // Determine LOD from detailLevels
//        int requiredLOD = 0;
//        for (int i = 0; i < detailLevels.size(); i++) {
//            if (distanceToNearestEdge > detailLevels[i].visibleDstThreshold) {
//                requiredLOD = detailLevels[i].lod;
//            }
//            else {
//                break;
//            }
//        }
//
//        if (chunk.currentLOD != requiredLOD || !chunk.hasData) {
//            if (!chunk.hasData) {
//                // Generate initial LOD
//                chunkDataThreadStart(requiredLOD, chunkPos, [this, &chunk](Chunk&) {
//                    chunk.updateGPUResources();
//                    });
//            }
//            else {
//                // LOD changed - generate new data WITHOUT CLEANUP
//                chunkDataThreadStart(requiredLOD, chunkPos, [this, &chunk](Chunk&) {
//                    chunk.cleanup();  // Delete old GPU resources
//                    chunk.updateGPUResources(); // Upload new data and clear CPU data
//                    });
//            }
//            chunk.currentLOD = requiredLOD;
//        }
//    }
//
//    // Keep your existing visibility handling
//    if (chunk.getLoaded() != visible) {
//        if (visible) {
//            visibleChunks.insert(&chunk);
//        }
//        else {
//            visibleChunks.erase(&chunk);
//        }
//    }
//    chunk.setLoaded(visible);
//}
//
//void Terrain::updateReceivedChunk(glm::vec2 chunkPos) {
//    float camPosX = camera->getCameraPosition().x;
//    float camPosZ = camera->getCameraPosition().z;
//
//
//    int currentChunkCoordX = floor(camPosX / chunkSize);
//    int currentChunkCoordZ = floor(camPosZ / chunkSize);
//    updateChunk(chunks[chunkPos], currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
//
//}
//
//void LODChunk::onChunkDataReceived(Chunk& chunk) {
//    // Ensure we have data to work with
//    if (chunk.getVertices().empty() || chunk.getNormals().empty()) {
//        throw std::runtime_error("Cannot create GPU resources without vertex data");
//    }
//
//    // Create resources
//    chunk.vbo = std::make_unique<VBO>(chunk.getVertices(), chunk.getNormals());
//    chunk.vao = std::make_unique<TerrainVao>(*chunk.vbo);
//
//    if (!chunk.getIndices().empty()) {
//        chunk.vao->Bind();
//        chunk.ibo = std::make_unique<IBO>(chunk.getIndices());
//        chunk.vao->Unbind();
//    }
//
//    if (!hasMesh) {
//        hasMesh = true;
//    }
//
//}
//
//void Terrain::chunkDataThreadStart(int lod, glm::vec2 pos, std::function<void(Chunk&)> callbackFunc) {
//    std::thread([this, lod, pos, callbackFunc]() {
//        Chunk tempChunk(vertexWidthAmount, vertexHeightAmount, octaveCount,
//            scalingBias, pos, detailLevels);
//        tempChunk.generateChunkData(lod);
//
//        {
//            std::lock_guard<std::mutex> lock(queueMutex);
//            if (auto it = chunks.find(pos); it != chunks.end()) {
//                // Move generated data to main chunk
//                it->second.vertices = std::move(tempChunk.vertices);
//                it->second.normals = std::move(tempChunk.normals);
//                it->second.indices = std::move(tempChunk.indices);
//                it->second.setNumStrips(tempChunk.getNumStrips()); // ADD THIS
//                it->second.setNumVertsPerStrip(tempChunk.getNumVertsPerStrip()); // ADD THIS
//                it->second.hasData = true;
//
//                // Queue callback
//                mapDataThreadInfoQueue.emplace(std::move(callbackFunc), pos);
//            }
//        }
//        }).detach();
//}
//
//void Chunk::updateGPUResources() {
//    if (vao) {
//        vao->Delete();
//        ibo->Delete();
//        vbo->Delete();
//    }
//
//    // Upload new data
//    vbo = std::make_unique<VBO>(vertices, normals);
//    vao = std::make_unique<TerrainVao>(*vbo);
//    if (!indices.empty()) {
//        vao->Bind();
//        ibo = std::make_unique<IBO>(indices);
//        vao->Unbind();
//    }
//
//    // Clear CPU data
//    vertices.clear();
//    vertices.shrink_to_fit();
//    normals.clear();
//    normals.shrink_to_fit();
//    indices.clear();
//    indices.shrink_to_fit();
//}
//
//void Chunk::cleanup() {
//    if (vao) vao->Delete();
//    if (ibo) ibo->Delete();
//    if (vbo) vbo->Delete();
//
//    // Remove the lines that clear vertices, normals, and indices
//    noiseSeed2D.clear();
//    perlinNoise2D.clear();
//    hasData = false;
//}
//
//void Chunk::generateChunkData(int lod) {
//    vertices.clear();
//    vertices.shrink_to_fit();
//    normals.clear();
//    normals.shrink_to_fit();
//    indices.clear();
//    indices.shrink_to_fit();
//    if (perlinNoise2D.empty())
//        //createPerlinNoise2D();
//        GenerateNoiseMap();
//
//
//    unsigned int meshSimplificationIncrement = (lod == 0) ? 1 : lod * 2;
//    int verticesPerLine = ((vertexWidthAmount - 1) / meshSimplificationIncrement) + 1;
//
//    numStrips = (vertexHeightAmount - 1) / meshSimplificationIncrement;
//    numVertsPerStrip = verticesPerLine * 2;  // each strip consists of squares, each square is 2 triangles (2 lines)
//
//    // Generate vertices
//    for (unsigned int i = 0; i < vertexWidthAmount; i += meshSimplificationIncrement) {
//        for (unsigned int j = 0; j < vertexHeightAmount; j += meshSimplificationIncrement) {
//            float y = (float)((perlinNoise2D[j * vertexWidthAmount + i]));
//            vertices.push_back(glm::vec3(i, y, j));
//        }
//    }
//
//    for (unsigned int i = 0; i < verticesPerLine - 1; i++)     // for each row (strip)
//        for (unsigned int j = 0; j < verticesPerLine; j++)    // for each column
//            for (unsigned int k = 0; k < 2; k++)            // for each side of the strip (leftand right primitives in square)
//                indices.push_back(verticesPerLine * (i + k) + j);   // top left -> top right -> bottom left -> bottom right
//
//
//    normals.resize(vertices.size(), glm::vec3(0));
//    for (size_t i = 0; i + 2 < indices.size(); i++) {
//        int index1 = indices[i];
//        int index2 = indices[i + 1];
//        int index3 = indices[i + 2];
//
//        // Calculate the normal for this triangle
//        auto edge1 = vertices[index2] - vertices[index1];
//        auto edge2 = vertices[index3] - vertices[index1];
//        auto normal = glm::cross(edge1, edge2);
//
//        // Ensure the normal faces the correct direction
//        if (normal.y < 0) {
//            normal = -normal;
//        }
//
//        normals[index1] += normal;
//        normals[index2] += normal;
//        normals[index3] += normal;
//    }
//
//    for (size_t i = 0; i < normals.size(); i++) {
//        normals[i] = glm::normalize(normals[i]);
//    }
//
//    perlinNoise2D.clear();
//    perlinNoise2D.shrink_to_fit();
//    noiseSeed2D.clear();
//    noiseSeed2D.shrink_to_fit();
//}
//
//void Chunk::GenerateNoiseMap() {
//    this->perlinNoise2D.resize(vertexWidthAmount * vertexHeightAmount);
//
//
//    // random values generation
//    std::mt19937 gen(9125123);
//    std::uniform_real_distribution<float> dist(0.0, 1.0);
//    this->noiseSeed2D.resize(vertexWidthAmount * vertexHeightAmount);
//    for (unsigned int i = 0; i < vertexWidthAmount * vertexHeightAmount; i++) {
//        this->noiseSeed2D[i] = dist(gen); // [0.0, 1.0]
//    }
//
//    float chunkScale = 590.0f;
//    float amplitude = 1.0f; // controls the strength of each octave contribution scalingBias - how much amplitude decreases for each octave
//    float frequency = 1.0f; // controls details for each octave
//    float lacunarity = 2.0f; // how much frequency increases for each octave
//
//    float maxPossibleHeight = 0.0f;
//    // calculate maximum possible height for furhter normalization
//    for (int oct = 0; oct < octaveCount; oct++) {
//        maxPossibleHeight += amplitude;
//        amplitude *= scalingBias;
//    }
//
//    // generate Perlin noise
//    for (int x = 0; x < vertexWidthAmount; x++) {
//        for (int y = 0; y < vertexHeightAmount; y++) {
//            float noiseHeight = 0.0f;
//            amplitude = 1.0f;
//            frequency = 1.0f;
//
//            // Iterate through octaves
//            for (int oct = 0; oct < octaveCount; oct++) {
//                // Calculate global sample coordinates
//                float sampleX = (x + position.x * (vertexWidthAmount - 1)) / chunkScale * frequency;
//                float sampleY = (y + position.y * (vertexHeightAmount - 1)) / chunkScale * frequency;
//
//                // interpolation factors
//                float blendX = fade(sampleX - std::floor(sampleX));
//                float blendY = fade(sampleY - std::floor(sampleY));
//
//                auto wrap = [](float value, int size) {
//                    return std::fmod(std::fmod(value, size) + size, size);  // adding size avoids negative values
//                    };
//                // get close to each other points
//                int wrappedX1 = wrap(sampleX, vertexWidthAmount);
//                int wrappedX2 = wrap(sampleX + 1, vertexWidthAmount);
//                int wrappedY1 = wrap(sampleY, vertexHeightAmount);
//                int wrappedY2 = wrap(sampleY + 1, vertexHeightAmount);
//
//                // linear interpolation
//                float sampleT = lerp(noiseSeed2D[wrappedY1 * vertexWidthAmount + wrappedX1],
//                    noiseSeed2D[wrappedY1 * vertexWidthAmount + wrappedX2], blendX);
//                float sampleB = lerp(noiseSeed2D[wrappedY2 * vertexWidthAmount + wrappedX1],
//                    noiseSeed2D[wrappedY2 * vertexWidthAmount + wrappedX2], blendX);
//                noiseHeight += lerp(sampleT, sampleB, blendY) * amplitude;
//
//                amplitude *= scalingBias;
//                frequency *= lacunarity;
//            }
//            perlinNoise2D[y * vertexWidthAmount + x] = noiseHeight;
//        }
//    }
//
//    // normalize noise values to [0, 256]
//    for (int x = 0; x < vertexWidthAmount; x++) {
//        for (int y = 0; y < vertexHeightAmount; y++) {
//            float normalizedHeight = (perlinNoise2D[y * vertexWidthAmount + x]) / (maxPossibleHeight);
//            perlinNoise2D[y * vertexWidthAmount + x] = std::clamp(normalizedHeight, 0.0f, 1.0f) * 1200.0f;
//        }
//    }
//
//
//}
//
//bool const Chunk::operator==(const Chunk& other) {
//    return(position == other.position);
//}
//
//float Chunk::lerp(float a, float b, float t) {
//    return a + (b - a) * t;
//}
//
//float Chunk::fade(float t) {
//    return t * t * t * (t * (t * 6 - 15) + 10);
//    //return t\* t\* (3 - 2 \* t);
//}
//
//void Terrain::updateThread() {
//    std::queue<std::pair<std::function<void(Chunk&)>, glm::vec2>> processingQueue;
//
//    // Quickly swap the queue to minimize lock time
//    {
//        std::lock_guard<std::mutex> lock(queueMutex);
//        processingQueue.swap(mapDataThreadInfoQueue);
//    }
//
//    while (!processingQueue.empty()) {
//        auto& task = processingQueue.front();
//        auto it = chunks.find(task.second);
//        if (it != chunks.end()) {
//            Chunk& chunk = it->second;
//            task.first(chunk); // This calls updateGPUResources, which clears CPU data
//        }
//        processingQueue.pop();
//    }
//}
//Chunk::Chunk(unsigned int vertexWidthAmount, unsigned int vertexHeightAmount,
//    unsigned int octaveCount, float scalingBias, glm::vec2 position,
//    std::vector<LODInfo> detailLevels) :
//    vertexWidthAmount(vertexWidthAmount),
//    vertexHeightAmount(vertexHeightAmount),
//    octaveCount(octaveCount),
//    scalingBias(scalingBias),
//    position(position),
//    loaded(false),
//    previousLodIndex(-1),
//    vbo(nullptr),
//    vao(nullptr),
//    ibo(nullptr)
//{}
//
//Chunk::~Chunk() {
//    cleanup(); // Explicitly release GPU resources if not already done
//    vertices.clear();
//    vertices.shrink_to_fit();
//    normals.clear();
//    normals.shrink_to_fit();
//    indices.clear();
//    indices.shrink_to_fit();
//    noiseSeed2D.clear();
//    perlinNoise2D.clear();
//}

#include "Terrain/Terrain.h"

Terrain::Terrain(Camera* camera, DB* db, unsigned int vertexWidthAmount, unsigned int vertexHeightAmount, unsigned int octaveCount, float scalingBias) : db(db){
    this->camera = camera;
    this->vertexWidthAmount = vertexWidthAmount;
    this->vertexHeightAmount = vertexHeightAmount;
    this->octaveCount = octaveCount;
    this->scalingBias = scalingBias;


    //double distToChunkEdge = sqrt((vertexWidthAmount - 1) * (vertexWidthAmount - 1) + (vertexHeightAmount - 1) * (vertexHeightAmount - 1));
    double distToChunkEdge = (vertexWidthAmount - 1);
    //detailLevels = { LODInfo(8, distToChunkEdge * 8) };

    detailLevels = { LODInfo(0, distToChunkEdge * 2), LODInfo(2, distToChunkEdge * 4), LODInfo(8, distToChunkEdge * 6) };
    //detailLevels = { LODInfo(0, distToChunkEdge * 4), LODInfo(2, distToChunkEdge * 5), LODInfo(4, distToChunkEdge * 8), LODInfo(6, distToChunkEdge * 10), LODInfo(8, distToChunkEdge * 12) };
    //detailLevels = { LODInfo(0, distToChunkEdge * 4), LODInfo(1,distToChunkEdge * 6), LODInfo(2,distToChunkEdge * 8) };

    std::sort(detailLevels.begin(), detailLevels.end(),
        [](const LODInfo& a, const LODInfo& b) {
            return a.visibleDstThreshold < b.visibleDstThreshold;
        });

    this->maxViewDst = detailLevels[detailLevels.size() - 1].visibleDstThreshold;
    this->chunkSize = vertexWidthAmount - 1;
    //this->chunksVisibleInViewDst = int(maxViewDst) / chunkSize;
    this->chunksVisibleInViewDst = static_cast<int>(std::ceil(maxViewDst / chunkSize));
}

Terrain::Terrain(const Terrain& other) :
    camera(other.camera),
    vertexWidthAmount(other.vertexWidthAmount),
    vertexHeightAmount(other.vertexHeightAmount),
    octaveCount(other.octaveCount),
    scalingBias(other.scalingBias),
    maxViewDst(other.maxViewDst),
    chunkSize(other.chunkSize),
    chunksVisibleInViewDst(other.chunksVisibleInViewDst),
    detailLevels(other.detailLevels)
{
    // Manually reconstruct chunks
    for (const auto& pair : other.chunks) {
        const glm::vec2& pos = pair.first;
        const Chunk& otherChunk = pair.second;

        // Construct new chunk in place
        auto [it, inserted] = chunks.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(pos),
            std::forward_as_tuple(
                otherChunk.getVertexWidthAmount(),
                otherChunk.getVertexHeightAmount(),
                otherChunk.getOctaveCount(),
                otherChunk.getScalingBias(),
                pos,
                detailLevels
            )
        );

        // Rebuild visibility
        if (otherChunk.getLoaded()) {
            visibleChunks.insert(&it->second);
        }
    }
}

Terrain& Terrain::operator=(const Terrain& other) {
    if (this != &other) {
        // Copy simple members
        camera = other.camera;
        vertexWidthAmount = other.vertexWidthAmount;
        vertexHeightAmount = other.vertexHeightAmount;
        octaveCount = other.octaveCount;
        scalingBias = other.scalingBias;
        maxViewDst = other.maxViewDst;
        chunkSize = other.chunkSize;
        chunksVisibleInViewDst = other.chunksVisibleInViewDst;
        detailLevels = other.detailLevels;

        // Rebuild chunks from scratch (don't copy)
        for (const auto& pair : other.chunks) {
            const glm::vec2& pos = pair.first;
            const Chunk& otherChunk = pair.second;

            // Create new chunk with same parameters
            chunks.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(pos),
                std::forward_as_tuple(
                    otherChunk.getVertexWidthAmount(),
                    otherChunk.getVertexHeightAmount(),
                    otherChunk.getOctaveCount(),
                    otherChunk.getScalingBias(),
                    pos,
                    detailLevels
                )
            );

            // If the other chunk was visible, add to visible list
            if (otherChunk.getLoaded()) {
                visibleChunks.insert(&chunks.at(pos));
            }
        }

        // Clear the thread queue (thread operations can't be copied)
        mapDataThreadInfoQueue = {};
    }
    return *this;


}

void Terrain::updateVisibleChunks() {
    float camPosX = camera->getCameraPosition().x;
    float camPosZ = camera->getCameraPosition().z;

    // Use ROUND instead of FLOOR to center chunk detection
    int currentChunkCoordX = static_cast<int>(std::floor(camPosX / chunkSize));
    int currentChunkCoordZ = static_cast<int>(std::floor(camPosZ / chunkSize));

    std::unordered_set<glm::vec2> visible;

    std::lock_guard<std::mutex> lock(queueMutex);


    for (int yOffset = -chunksVisibleInViewDst; yOffset <= chunksVisibleInViewDst; yOffset++) {
       for (int xOffset = -chunksVisibleInViewDst; xOffset <= chunksVisibleInViewDst; xOffset++) {
            glm::vec2 viewedChunkCoord(currentChunkCoordX + xOffset, currentChunkCoordZ + yOffset);

            visible.insert(viewedChunkCoord);

            if (chunks.find(viewedChunkCoord) == chunks.end()) {
                chunks.try_emplace(viewedChunkCoord,
                    Chunk(vertexWidthAmount, vertexHeightAmount, octaveCount,
                        scalingBias, viewedChunkCoord, detailLevels));
            }
        }
    }

    // Update visibility for ALL existing chunks
    for (auto& [coord, chunk] : chunks) {
        updateChunk(chunk, currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
    }

    for (auto it = chunks.begin(); it != chunks.end();) {
        if (!visible.count(it->first)) {
            if (it->second.vao) it->second.vao->Delete();
            if (it->second.ibo) it->second.ibo->Delete();
            if (it->second.vbo) it->second.vbo->Delete();
            it->second.vao.reset();
            it->second.ibo.reset();
            it->second.vbo.reset();
            it->second.setLoaded(false);
            it->second.setPreviousLodIndex(-1);
            it->second.setNumStrips(0);
            it->second.setNumVertsPerStrip(0);

            it = chunks.erase(it);
        }
        else {
            ++it;
        }
    }

}

void Terrain::updateChunk(Chunk& chunk, int currChunkCoordX, int currChunkCoordZ, float camPosX, float camPosZ) {
    glm::vec2 chunkPos = chunk.getPosition();

    // World position of chunk's edges
    float xMin = chunkPos.x * chunkSize;
    float xMax = xMin + chunkSize;
    float zMin = chunkPos.y * chunkSize;
    float zMax = zMin + chunkSize;

    // Distance calculations (keep your existing code)
    float dx = std::max(0.0f, std::max(xMin - camPosX, camPosX - xMax));
    float dz = std::max(0.0f, std::max(zMin - camPosZ, camPosZ - zMax));
    float distanceToNearestEdge = std::sqrt(dx * dx + dz * dz);

    bool visible = (distanceToNearestEdge <= maxViewDst);

    if (visible) {
        // Determine LOD from detailLevels
        int requiredLOD = 0;
        for (int i = 0; i < detailLevels.size(); i++) {
            if (distanceToNearestEdge > detailLevels[i].visibleDstThreshold) {
                requiredLOD = detailLevels[i].lod;
            }
            else {
                break;
            }
        }

        if (chunk.currentLOD != requiredLOD || !chunk.hasData) {
            if (!chunk.hasData) {
                // Generate initial LOD
                chunkDataThreadStart(requiredLOD, chunkPos, [this, &chunk](Chunk&) {
                    
                    if (chunk.getVertices().empty() || chunk.getNormals().empty()) {
                        std::cerr << " hasn't data ERROR: Empty positions/normals vector!\n";
                        std::cerr << "Positions size: " << chunk.getVertices().size() << "\n";
                        std::cerr << "Normals size: " << chunk.getNormals().size() << "\n";
                        return; // or handle the error
                    }

                    chunk.updateGPUResources();
                    });
            }
            else {
                // LOD changed - generate new data WITHOUT CLEANUP
                chunkDataThreadStart(requiredLOD, chunkPos, [this, &chunk](Chunk&) {
                    chunk.cleanup();  // Delete old GPU resources

                        if (chunk.getVertices().empty() || chunk.getNormals().empty()) {
                            std::cerr << " hasData ERROR: Empty positions/normals vector!\n";
                            std::cerr << "Positions size: " << chunk.getVertices().size() << "\n";
                            std::cerr << "Normals size: " << chunk.getNormals().size() << "\n";
                            return; // or handle the error
                        }


                    chunk.updateGPUResources(); // Upload new data and clear CPU data
                    });
            }

            chunk.currentLOD = requiredLOD;
        }
    }

    // Keep your existing visibility handling
    if (chunk.getLoaded() != visible) {
        if (visible) {
            visibleChunks.insert(&chunk);
        }
        else {
            visibleChunks.erase(&chunk);
        }
    }
    chunk.setLoaded(visible);
}

void Terrain::updateReceivedChunk(glm::vec2 chunkPos) {
    float camPosX = camera->getCameraPosition().x;
    float camPosZ = camera->getCameraPosition().z;


    int currentChunkCoordX = floor(camPosX / chunkSize);
    int currentChunkCoordZ = floor(camPosZ / chunkSize);
    updateChunk(chunks[chunkPos], currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);

}

void LODChunk::onChunkDataReceived(Chunk& chunk) {
    //// Ensure we have data to work with
    if (chunk.getVertices().empty() || chunk.getNormals().empty()) {
        throw std::runtime_error("Cannot create GPU resources without vertex data");
    }

    // Create resources
    chunk.vbo = std::make_unique<VBO>(chunk.getVertices(), chunk.getNormals());
    chunk.vao = std::make_unique<TerrainVao>(*chunk.vbo);

    if (!chunk.getIndices().empty()) {
        chunk.vao->Bind();
        chunk.ibo = std::make_unique<IBO>(chunk.getIndices());
        chunk.vao->Unbind();
    }

    if (!hasMesh) {
        hasMesh = true;
    }

}

void Terrain::chunkDataThreadStart(int lod, glm::vec2 pos, std::function<void(Chunk&)> callbackFunc) {
    std::thread([this, lod, pos, callbackFunc]() {
        Chunk tempChunk(vertexWidthAmount, vertexHeightAmount, octaveCount,
            scalingBias, pos, detailLevels);

        // Attempt to load heightmap from DB
        std::vector<float> noiseData;
        bool loaded = db->loadHeightMap(pos, octaveCount, scalingBias,
            vertexWidthAmount, vertexHeightAmount, noiseData);

        if (loaded && noiseData.size() == vertexWidthAmount * vertexHeightAmount) {
            tempChunk.perlinNoise2D = std::move(noiseData);
        }
        else {
            // Generate new noise and store in DB
            tempChunk.GenerateNoiseMap();
            db->storeHeightMap(pos, octaveCount, scalingBias,
                vertexWidthAmount, vertexHeightAmount,
                tempChunk.perlinNoise2D.data(),
                tempChunk.perlinNoise2D.size());
        }

        tempChunk.generateChunkData(lod);

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (auto it = chunks.find(pos); it != chunks.end()) {
                it->second.vertices = std::move(tempChunk.vertices);
                it->second.normals = std::move(tempChunk.normals);
                it->second.indices = std::move(tempChunk.indices);
                it->second.hasData = true;

                mapDataThreadInfoQueue.emplace(std::move(callbackFunc), pos);
            }
        }
        }).detach();
}

void Chunk::updateGPUResources() {
    if (vao) {
        vao->Delete();
        ibo->Delete();
        vbo->Delete();
    }
    numStrips = 0;
    numVertsPerStrip = 0;


    //if (vertices.empty() || normals.empty()) {
    //    std::cerr << " updateGPURes ERROR: Empty positions/normals vector!\n";
    //    std::cerr << "Positions size: " << vertices.size() << "\n";
    //    std::cerr << "Normals size: " << normals.size() << "\n";
    //    //return; // or handle the error
    //}

    // Upload new data
    vbo = std::make_unique<VBO>(vertices, normals);
    vao = std::make_unique<TerrainVao>(*vbo);
    if (!indices.empty()) {
        vao->Bind();
        ibo = std::make_unique<IBO>(indices);
        vao->Unbind();
    }

    unsigned int meshSimplificationIncrement = (currentLOD == 0) ? 1 : currentLOD * 2;
    int verticesPerLine = ((vertexWidthAmount - 1) / meshSimplificationIncrement) + 1;

    numStrips = (vertexHeightAmount - 1) / meshSimplificationIncrement;
    numVertsPerStrip = verticesPerLine * 2;  // each strip consists of squares, each square is 2 triangles (2 lines)

    // Clear CPU data
    vertices.clear();
    vertices.shrink_to_fit();
    normals.clear();
    normals.shrink_to_fit();
    indices.clear();
    indices.shrink_to_fit();
}

void Chunk::cleanup() {
    // Remove the lines that clear vertices, normals, and indices
    noiseSeed2D.clear();
    perlinNoise2D.clear();
    hasData = false;
}

void Chunk::generateChunkData(int lod) {
    vertices.clear();
    vertices.shrink_to_fit();
    normals.clear();
    normals.shrink_to_fit();
    indices.clear();
    indices.shrink_to_fit();
    if (perlinNoise2D.empty())
        //createPerlinNoise2D();
        GenerateNoiseMap();


    //unsigned int meshSimplificationIncrement = (lod == 0) ? 1 : lod * 2;
    //// Ensure meshSimplificationIncrement doesn't cause verticesPerLine < 2
    //while (((vertexWidthAmount - 1) / meshSimplificationIncrement) + 1 < 2) {
    //    meshSimplificationIncrement /= 2;
    //    if (meshSimplificationIncrement < 1) {
    //        meshSimplificationIncrement = 1;
    //        break;
    //    }
    //}

    //int verticesPerLine = ((vertexWidthAmount - 1) / meshSimplificationIncrement) + 1;


    unsigned int meshSimplificationIncrement = (lod == 0) ? 1 : lod * 2;
    int verticesPerLine = ((vertexWidthAmount - 1) / meshSimplificationIncrement) + 1;

    // Generate vertices
    for (unsigned int i = 0; i < vertexWidthAmount; i += meshSimplificationIncrement) {
        for (unsigned int j = 0; j < vertexHeightAmount; j += meshSimplificationIncrement) {
            float y = (float)((perlinNoise2D[j * vertexWidthAmount + i]));
            vertices.push_back(glm::vec3(i, y, j));
        }
    }

    for (unsigned int i = 0; i < verticesPerLine - 1; i++)     // for each row (strip)
        for (unsigned int j = 0; j < verticesPerLine; j++)    // for each column
            for (unsigned int k = 0; k < 2; k++)            // for each side of the strip (leftand right primitives in square)
                indices.push_back(verticesPerLine * (i + k) + j);   // top left -> top right -> bottom left -> bottom right


    normals.resize(vertices.size(), glm::vec3(0));
    for (size_t i = 0; i + 2 < indices.size(); i++) {
        int index1 = indices[i];
        int index2 = indices[i + 1];
        int index3 = indices[i + 2];

        // Calculate the normal for this triangle
        auto edge1 = vertices[index2] - vertices[index1];
        auto edge2 = vertices[index3] - vertices[index1];
        auto normal = glm::cross(edge1, edge2);

        // Ensure the normal faces the correct direction
        if (normal.y < 0) {
            normal = -normal;
        }

        normals[index1] += normal;
        normals[index2] += normal;
        normals[index3] += normal;
    }

    for (size_t i = 0; i < normals.size(); i++) {
        normals[i] = glm::normalize(normals[i]);
    }

    perlinNoise2D.clear();
    perlinNoise2D.shrink_to_fit();
    noiseSeed2D.clear();
    noiseSeed2D.shrink_to_fit();
}

void Chunk::GenerateNoiseMap() {
    this->perlinNoise2D.resize(vertexWidthAmount * vertexHeightAmount);


    // random values generation
    std::mt19937 gen(9125123);
    std::uniform_real_distribution<float> dist(0.0, 1.0);
    this->noiseSeed2D.resize(vertexWidthAmount * vertexHeightAmount);
    for (unsigned int i = 0; i < vertexWidthAmount * vertexHeightAmount; i++) {
        this->noiseSeed2D[i] = dist(gen); // [0.0, 1.0]
    }

    float chunkScale = 590.0f;
    float amplitude = 1.0f; // controls the strength of each octave contribution scalingBias - how much amplitude decreases for each octave
    float frequency = 1.0f; // controls details for each octave
    float lacunarity = 2.0f; // how much frequency increases for each octave

    float maxPossibleHeight = 0.0f;
    // calculate maximum possible height for furhter normalization
    for (int oct = 0; oct < octaveCount; oct++) {
        maxPossibleHeight += amplitude;
        amplitude *= scalingBias;
    }

    // generate Perlin noise
    for (int x = 0; x < vertexWidthAmount; x++) {
        for (int y = 0; y < vertexHeightAmount; y++) {
            float noiseHeight = 0.0f;
            amplitude = 1.0f;
            frequency = 1.0f;

            // Iterate through octaves
            for (int oct = 0; oct < octaveCount; oct++) {
                // Calculate global sample coordinates
                float sampleX = (x + position.x * (vertexWidthAmount - 1)) / chunkScale * frequency;
                float sampleY = (y + position.y * (vertexHeightAmount - 1)) / chunkScale * frequency;

                // interpolation factors
                float blendX = fade(sampleX - std::floor(sampleX));
                float blendY = fade(sampleY - std::floor(sampleY));

                auto wrap = [](float value, int size) {
                    return std::fmod(std::fmod(value, size) + size, size);  // adding size avoids negative values
                    };
                // get close to each other points
                int wrappedX1 = wrap(sampleX, vertexWidthAmount);
                int wrappedX2 = wrap(sampleX + 1, vertexWidthAmount);
                int wrappedY1 = wrap(sampleY, vertexHeightAmount);
                int wrappedY2 = wrap(sampleY + 1, vertexHeightAmount);

                // linear interpolation
                float sampleT = lerp(noiseSeed2D[wrappedY1 * vertexWidthAmount + wrappedX1],
                    noiseSeed2D[wrappedY1 * vertexWidthAmount + wrappedX2], blendX);
                float sampleB = lerp(noiseSeed2D[wrappedY2 * vertexWidthAmount + wrappedX1],
                    noiseSeed2D[wrappedY2 * vertexWidthAmount + wrappedX2], blendX);
                noiseHeight += lerp(sampleT, sampleB, blendY) * amplitude;

                amplitude *= scalingBias;
                frequency *= lacunarity;
            }
            perlinNoise2D[y * vertexWidthAmount + x] = noiseHeight;
        }
    }

    // normalize noise values to [0, 256]
    for (int x = 0; x < vertexWidthAmount; x++) {
        for (int y = 0; y < vertexHeightAmount; y++) {
            float normalizedHeight = (perlinNoise2D[y * vertexWidthAmount + x]) / (maxPossibleHeight);
            perlinNoise2D[y * vertexWidthAmount + x] = std::clamp(normalizedHeight, 0.0f, 1.0f) * 1200.0f;
        }
    }


}

bool const Chunk::operator==(const Chunk& other) {
    return(position == other.position);
}

float Chunk::lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float Chunk::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
    //return t\* t\* (3 - 2 \* t);
}

void Terrain::updateThread() {
    std::queue<std::pair<std::function<void(Chunk&)>, glm::vec2>> processingQueue;

    // Quickly swap the queue to minimize lock time
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        processingQueue.swap(mapDataThreadInfoQueue);
    }

    while (!processingQueue.empty()) {
        auto& task = processingQueue.front();
        auto it = chunks.find(task.second);
        if (it != chunks.end()) {
            Chunk& chunk = it->second;
            task.first(chunk); // This calls updateGPUResources, which clears CPU data
        }
        processingQueue.pop();
    }
}
Chunk::Chunk(unsigned int vertexWidthAmount, unsigned int vertexHeightAmount,
    unsigned int octaveCount, float scalingBias, glm::vec2 position,
    std::vector<LODInfo> detailLevels) :
    vertexWidthAmount(vertexWidthAmount),
    vertexHeightAmount(vertexHeightAmount),
    octaveCount(octaveCount),
    scalingBias(scalingBias),
    position(position),
    loaded(false),
    previousLodIndex(-1),
    vbo(nullptr),
    vao(nullptr),
    ibo(nullptr)
{}

Chunk::~Chunk() {
    cleanup();
}



glm::vec2 Chunk::getPosition() {
    return position;
}

unsigned int Chunk::getVertexWidthAmount() const{
    return vertexWidthAmount;
}

unsigned int Chunk::getVertexHeightAmount() const {
    return vertexHeightAmount;
}

unsigned int Chunk::getOctaveCount() const {
    return octaveCount;
}

float Chunk::getScalingBias() const{
    return scalingBias;
}

std::vector<glm::vec3> Chunk::getVertices() {
    return vertices;
}

std::vector<unsigned int> Chunk::getIndices() {
    return indices;
}

std::vector<glm::vec3> Chunk::getNormals() {
    return normals;
}

std::vector<float>& Chunk::getNoiseSeed2D() {
    return noiseSeed2D;
}

void Chunk::setLoaded(bool loaded) {
    this->loaded = loaded;
}

bool Chunk::getLoaded() const{
    return loaded;
}

void Chunk::setPerlinNoise2D(const std::vector<float>& data) {
    this->perlinNoise2D = data;
}
void Chunk::setNoiseSeed2D(const std::vector<float>& data) {
    this->noiseSeed2D = data;
}
std::vector<float>& Chunk::getPerlinNoise2D() {
    return perlinNoise2D;
}


std::vector<glm::vec3>* Chunk::getVerticesPtr() {
    return &vertices;
}
void Chunk::setVertices(std::vector<glm::vec3> vertices) {
    this->vertices = vertices;
}


std::vector<unsigned int>* Chunk::getIndicesPtr() {
    return &indices;
}
void Chunk::setIndices(std::vector<unsigned int> indices) {
    this->indices = indices;
}

std::vector<glm::vec3>* Chunk::getNormalsPtr() {
    return &normals;
}
void Chunk::setNormals(std::vector<glm::vec3> normals) {
    this->normals = normals;
}

void Chunk::setVBO(std::unique_ptr<VBO> newVbo) {
    vbo = std::move(newVbo);
}

VBO& Chunk::getVBO() {  // Return raw pointer, not the unique_ptr
    return *vbo;
}


void Chunk::setIBO(std::unique_ptr<IBO> newIBO) {
    ibo = std::move(newIBO);
}

IBO& Chunk::getIBO() {  // Return raw pointer, not the unique_ptr
    return *ibo;
}

void Chunk::setVAO(std::unique_ptr<TerrainVao> vao) {
    vao = std::move(vao);
}

TerrainVao& Chunk::getVAO() {  // Return raw pointer, not the unique_ptr
    return *vao;
}



int Chunk::getPreviousLodIndex() {
    return previousLodIndex;
}
unsigned int Chunk::getNumStrips() {
    return numStrips;
}
unsigned int Chunk::getNumVertsPerStrip() {
    return numVertsPerStrip;
}

void Chunk::setPreviousLodIndex(int previousLodIndex) {
    this->previousLodIndex = previousLodIndex;
}

void Chunk::setNumStrips(unsigned int numStrips) {
    this->numStrips = numStrips;
}
void Chunk::setNumVertsPerStrip(unsigned int numVertsPerStrip) {
    this->numVertsPerStrip = numVertsPerStrip;
}

void LODChunk::setHasMesh(bool hasMesh) {
    this->hasMesh = hasMesh;
}

//LODChunk& Chunk::getLodChunk(int elementId) {
//    return lodChunks[elementId];
//};
//
//LODChunk* Chunk::getLodChunkPtr(int elementId) {
//    return &lodChunks[elementId];
//};
//
//const std::vector<LODChunk>& Chunk::getLodChunks(){
//    return lodChunks;
//}

void Chunk::setPosition(glm::vec2 position) {
    this->position = position;
}

LODChunk::LODChunk(int lod)
    : lod(lod),
    Mesh(std::make_unique<Chunk>()), // Initialize with empty chunk
    hasRequestedMesh(false),
    hasMesh(false) {}

bool LODChunk::getHasChunk() {
    return hasMesh;
}
bool LODChunk::getHasRequestedChunk() {
    return hasRequestedMesh;
}
int LODChunk::getLOD() {
    return lod;
}

void LODChunk::setHasRequestedChunk(bool hasRequestedChunk) {
    this->hasRequestedMesh = hasRequestedChunk;
}

Chunk* LODChunk::getChunk() const{
    return Mesh.get();
}

unsigned int Terrain::getVertexWidthAmount() {
    return vertexWidthAmount;
};
unsigned int Terrain::getVertexHeightAmount() {
    return vertexHeightAmount;
};

std::unordered_set<Chunk*> Terrain::getVisibleChunks() {
    return visibleChunks;
};