#include "Terrain.h"

Terrain::Terrain(Camera* camera, unsigned int vertexWidthAmount, unsigned int vertexHeightAmount, unsigned int octaveCount, float scalingBias) {
    this->camera = camera;
    this->vertexWidthAmount = vertexWidthAmount;
    this->vertexHeightAmount = vertexHeightAmount;
    this->octaveCount = octaveCount;
    this->scalingBias = scalingBias;

    double distToChunkEdge = sqrt(vertexWidthAmount * vertexWidthAmount + vertexHeightAmount * vertexHeightAmount);
    

    detailLevels = { LODInfo(0, distToChunkEdge * 2), LODInfo(1,distToChunkEdge * 3), LODInfo(2, distToChunkEdge * 4) };
    //detailLevels = { LODInfo(0, distToChunkEdge * 4), LODInfo(1,distToChunkEdge * 6), LODInfo(2,distToChunkEdge * 8) };
    this->maxViewDst = detailLevels[detailLevels.size() - 1].visibleDstThreshold;
    this->chunkSize = vertexWidthAmount - 1;
    this->chunksVisibleInViewDst = int(maxViewDst) / chunkSize;
}

Terrain::Terrain(const Terrain& other) : camera(other.camera), vertexWidthAmount(other.vertexWidthAmount), vertexHeightAmount(other.vertexHeightAmount),
                                        octaveCount(other.octaveCount), scalingBias(other.scalingBias), maxViewDst(other.maxViewDst),
                                        chunkSize(other.chunkSize), chunksVisibleInViewDst(other.chunksVisibleInViewDst), chunks(other.chunks),
                                        visibleChunks(other.visibleChunks), mapDataThreadInfoQueue(other.mapDataThreadInfoQueue),
                                        detailLevels(other.detailLevels) {}
                                        
Terrain& Terrain::operator=(const Terrain& other) {
    if (this != &other) {
        camera = other.camera;
        vertexWidthAmount = other.vertexWidthAmount; 
        vertexHeightAmount = other.vertexHeightAmount;
        octaveCount = other.octaveCount;
        scalingBias = other.scalingBias;
        maxViewDst = other.maxViewDst;
        chunkSize = other.chunkSize;
        chunksVisibleInViewDst = other.chunksVisibleInViewDst;
        chunks = other.chunks;
        visibleChunks = other.visibleChunks;
        mapDataThreadInfoQueue = other.mapDataThreadInfoQueue;
        detailLevels = other.detailLevels;
    }
    return *this;
}

unsigned int Terrain::getVertexWidthAmount() {
    return vertexWidthAmount;
};
unsigned int Terrain::getVertexHeightAmount() {
    return vertexHeightAmount;
};

std::vector<Chunk*> Terrain::getVisibleChunks() {
    return visibleChunks;
};


void Terrain::updateVisibleChunks() {
    float camPosX = camera->getCameraPosition().x;
    float camPosZ = camera->getCameraPosition().z;

    // current chunk corrdinates(every axis increments or decrements by 1)
    int currentChunkCoordX = floor(camPosX / chunkSize);
    int currentChunkCoordZ = floor(camPosZ / chunkSize);

    std::lock_guard<std::mutex> lock(queueMutex);
    // loop through all chunks in viewer distance
    for (int yOffset = -chunksVisibleInViewDst; yOffset <= chunksVisibleInViewDst; yOffset++) {
        for (int xOffset = -chunksVisibleInViewDst; xOffset <= chunksVisibleInViewDst; xOffset++) {
            // chunk that's being calculated
            glm::vec2 viewedChunkCoord = glm::vec2(currentChunkCoordX + xOffset, currentChunkCoordZ + yOffset);
            // if chunk has been already calculated
            if (chunks.find(viewedChunkCoord) != chunks.end()) {
                updateChunk(chunks[viewedChunkCoord], currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
            }
            else {
                chunks.emplace(viewedChunkCoord,
                    Chunk(vertexWidthAmount, vertexHeightAmount, octaveCount, scalingBias, maxViewDst, viewedChunkCoord, detailLevels));

                updateChunk(chunks[viewedChunkCoord], currentChunkCoordX, currentChunkCoordZ, camPosX, camPosZ);
            }
        }
    }
}

void Terrain::updateChunk(Chunk& chunk, int currChunkCoordX, int currChunkCoordZ, float camPosX, float camPosZ) {
    glm::vec2 chunkPos = chunk.getPosition();
    // chunk position in vertices coordinates
    float xPos = chunkPos.x * vertexWidthAmount + ((currChunkCoordX <= chunkPos.x) * vertexWidthAmount);
    float zPos = chunkPos.y * vertexHeightAmount + ((currChunkCoordZ <= chunkPos.y) * vertexHeightAmount);
    //float xPos = chunkPos.x * vertexWidthAmount + ((0 <= chunkPos.x) * vertexWidthAmount);
    //float zPos = chunkPos.y * vertexHeightAmount + ((0 <= chunkPos.y) * vertexHeightAmount);
    float viewerDstFromNearestEdge = distance(glm::vec3(camPosX, 0.0, camPosZ), glm::vec3(xPos, 0, zPos));
    
    bool wasLoaded = chunk.getLoaded();
    
    bool visible = (viewerDstFromNearestEdge <= maxViewDst);

    if (visible) {
        int lodIndex = 0;
        
        // set lod chunk's id
        for (int i = 0; i < detailLevels.size() - 1; i++) {
            if (viewerDstFromNearestEdge > detailLevels[i].visibleDstThreshold)
                lodIndex = i + 1;
            else
                break;
        }
        // if chunk should be changed
        if (lodIndex != chunk.getPreviousLodIndex()) {
            // if chunk with this lod has been loaded
            if (chunk.getLodChunk(lodIndex).getHasChunk()) {
                chunk.setPreviousLodIndex(lodIndex);

                chunk.setVBO(chunk.getLodChunk(lodIndex).getChunk()->getVBO());
                chunk.setVAO(chunk.getLodChunk(lodIndex).getChunk()->getVAO());
                chunk.setIBO(chunk.getLodChunk(lodIndex).getChunk()->getIBO());
                chunk.setNumStrips(chunk.getLodChunk(lodIndex).getChunk()->getNumStrips());
                chunk.setNumVertsPerStrip(chunk.getLodChunk(lodIndex).getChunk()->getNumVertsPerStrip());
            }
            // if chunk with this lod hasn't been created
            else if (!chunk.getLodChunk(lodIndex).getHasRequestedChunk()) {
                chunk.getLodChunkPtr(lodIndex)->setHasRequestedChunk(true);

                chunkDataThreadStart(lodIndex, chunkPos, [this, &chunk, lodIndex, currChunkCoordX, currChunkCoordZ, camPosX, camPosZ](Chunk&) {
                    chunk.getLodChunkPtr(lodIndex)->onChunkDataReceived(*chunk.getLodChunk(lodIndex).getChunk());
                    updateChunk(chunk, currChunkCoordX, currChunkCoordZ, camPosX, camPosZ);
                    });
            }
        }
    }

    if (wasLoaded != visible) {
        if (visible)
            visibleChunks.push_back(&chunk);
        else
            visibleChunks.erase(std::remove(visibleChunks.begin(), visibleChunks.end(), &chunks[chunk.getPosition()]), visibleChunks.end());
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
    chunk.setVBO(VBO(chunk.getVertices(), chunk.getNormals()));
    chunk.setVAO(TerrainVao(chunk.getVBO()));
    chunk.setIBO(IBO(chunk.getIndices()));
    chunk.getVerticesPtr()->clear();
    chunk.getIndicesPtr()->clear();
    chunk.getNormalsPtr()->clear();

    hasMesh = true;
}

void Terrain::chunkDataThreadStart(int lod, glm::vec2 pos, std::function<void(Chunk&)> callbackFunc) {
    std::thread thread([this, lod, pos, callbackFunc]() {

        Chunk chunk;
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            chunk = chunks[pos];
        }
        chunk.generateChunkData(chunks[pos].getLodChunk(lod).getLOD());

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            chunks[pos].setPerlinNoise2D(chunk.getPerlinNoise2D());

            chunks[pos].getLodChunk(lod).getChunk()->setVertices(chunk.getVertices());
            chunks[pos].getLodChunk(lod).getChunk()->setIndices(chunk.getIndices());
            chunks[pos].getLodChunk(lod).getChunk()->setNormals(chunk.getNormals());
            chunks[pos].getLodChunk(lod).getChunk()->setNumStrips(chunk.getNumStrips());
            chunks[pos].getLodChunk(lod).getChunk()->setNumVertsPerStrip(chunk.getNumVertsPerStrip());
            mapDataThreadInfoQueue.push(ChunkThreadData(callbackFunc, *chunks[pos].getLodChunk(lod).getChunk()));
        }
        });

    thread.detach();
}

void Chunk::generateChunkData(int lod) {
    if (perlinNoise2D == nullptr)
        createPerlinNoise2D();

    unsigned int meshSimplificationIncrement = (lod == 0) ? 1 : lod * 2;
    int verticesPerLine = ((vertexWidthAmount - 1) / meshSimplificationIncrement) + 1;

    numStrips = (vertexHeightAmount - 1) / meshSimplificationIncrement;
    numVertsPerStrip = verticesPerLine * 2;  // each strip consists of squares, each square is 2 triangles (2 lines)

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
}

void Chunk::createPerlinNoise2D() {
    this->noiseSeed2D = new float[vertexWidthAmount * vertexHeightAmount];

    std::random_device rd;
    std::mt19937 gen(rd());
    //std::mt19937 gen(12345);
    std::uniform_real_distribution<float> dist(0.0, 1.0);

    for (unsigned int i = 0; i < vertexWidthAmount * vertexHeightAmount; i++)
        this->noiseSeed2D[i] = dist(gen);	// random values [0.0, 1.0]

    this->perlinNoise2D = new float[vertexWidthAmount * vertexHeightAmount];

    float minNoiseHeight = 1;
    float maxNoiseHeight = 0;

    float scale1 = 1.0f;
    float maxPosH = 0.0f;

    for (int oct = 0; oct < octaveCount; oct++)
    {
        maxPosH += scale1;
        scale1 *= scalingBias;
    }

    for (int x = 0; x < vertexWidthAmount; x++) {
        for (int y = 0; y < vertexHeightAmount; y++)
        {
            float noise = 0.0f;
            float maxScale = 0.0f;

            float scale = 1.0f;    // determines how much lower the contribution for each octave is (makes higher octaves more significant effect)

            for (int oct = 0; oct < octaveCount; oct++)
            {
                int nPitchX = vertexWidthAmount >> oct;  // make octave more persistent every loop
                int nPitchY = vertexHeightAmount >> oct;

                if (nPitchX == 0)
                    nPitchX = 1;
                if (nPitchY == 0)
                    nPitchY = 1;

                int nSampleX1 = (x / nPitchX) * nPitchX;
                int nSampleX2 = (nSampleX1 + nPitchX) % vertexWidthAmount;  // '%' to make values wrap around

                int nSampleY1 = (y / nPitchY) * nPitchY;
                int nSampleY2 = (nSampleY1 + nPitchY) % vertexHeightAmount;

                float fBlendX = fade((float)(x - nSampleX1) / (float)nPitchX);
                float fBlendY = fade((float)(y - nSampleY1) / (float)nPitchY);
                float fSampleT = lerp(noiseSeed2D[nSampleY1 * vertexWidthAmount + nSampleX1], noiseSeed2D[nSampleY1 * vertexWidthAmount + nSampleX2], fBlendX);   // interpolate between 2 top values
                float fSampleB = lerp(noiseSeed2D[nSampleY2 * vertexWidthAmount + nSampleX1], noiseSeed2D[nSampleY2 * vertexWidthAmount + nSampleX2], fBlendX);   // interpolate between 2 bottom values

                maxScale += scale;    // maximum possible number for fitting output in range

                noise += lerp(fSampleT, fSampleB, fBlendY) * scale;   // interpolate between top and bottom interpolation results

                //scale *= scale / bias;
                scale /= scalingBias;
            }
            if (noise / maxScale > maxNoiseHeight)
                maxNoiseHeight = noise / maxScale;
            if (noise / maxScale < minNoiseHeight)
                minNoiseHeight = noise / maxScale;
            perlinNoise2D[y * vertexWidthAmount + x] = (noise / maxScale);   // make range of output between 0 and 1
            perlinNoise2D[y * vertexWidthAmount + x] *= 256;
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
    //return t;
    return t * t * t * (t * (t * 6 - 15) + 10);
    //return t* t* (3 - 2 * t);
}

void Terrain::updateThread() {
    while (!mapDataThreadInfoQueue.empty()) {
        std::lock_guard<std::mutex> lock(queueMutex);
        ChunkThreadData threadInfo = mapDataThreadInfoQueue.front();
        mapDataThreadInfoQueue.pop();
        threadInfo.callback(threadInfo.parameter);
    }
}

Chunk::Chunk(unsigned int vertexWidthAmount, unsigned int vertexHeightAmount, 
    unsigned int octaveCount, float scalingBias, float maxViewDst, glm::vec2 position, std::vector<LODInfo> detailLevels) {
    this->vertexWidthAmount = vertexWidthAmount;
    this->vertexHeightAmount = vertexHeightAmount;
    this->octaveCount = octaveCount;
    this->scalingBias = scalingBias;
    this->maxViewDst = maxViewDst;

    this->noiseSeed2D = nullptr;
    this->perlinNoise2D = nullptr;
    
    this->loaded = false;
    this->position = position;
    this->previousLodIndex = -1;

    for (int i = 0; i < detailLevels.size(); i++) 
        lodChunks.push_back(LODChunk(detailLevels[i].lod));
}


glm::vec2 Chunk::getPosition() {
    return position;
};

void Chunk::setLoaded(bool loaded) {
    this->loaded = loaded;
}

bool Chunk::getLoaded() {
    return loaded;
}

void Chunk::setNoiseSeed2D(float* noiseSeed2D) {
    this->noiseSeed2D = noiseSeed2D;
}
float* Chunk::getNoiseSeed2D() {
    return noiseSeed2D;
}

void Chunk::setPerlinNoise2D(float* perlinNoise2D) {
    this->perlinNoise2D = perlinNoise2D;
}
float* Chunk::getPerlinNoise2D() {
    return perlinNoise2D;
}

std::vector<glm::vec3> Chunk::getVertices() {
    return vertices;
}
std::vector<glm::vec3>* Chunk::getVerticesPtr() {
    return &vertices;
}
void Chunk::setVertices(std::vector<glm::vec3> vertices) {
    this->vertices = vertices;
}

std::vector<unsigned int> Chunk::getIndices() {
    return indices;
}
std::vector<unsigned int>* Chunk::getIndicesPtr() {
    return &indices;
}
void Chunk::setIndices(std::vector<unsigned int> indices) {
    this->indices = indices;
}

std::vector<glm::vec3> Chunk::getNormals() {
    return normals;
}
std::vector<glm::vec3>* Chunk::getNormalsPtr() {
    return &normals;
}
void Chunk::setNormals(std::vector<glm::vec3> normals) {
    this->normals = normals;
}

TerrainVao Chunk::getVAO() {
    return vao;
}

void Chunk::setVBO(VBO vbo) {
    this->vbo = vbo;
};
VBO Chunk::getVBO() {
    return vbo;
}

IBO Chunk::getIBO() {
    return ibo;
}

void Chunk::setVAO(TerrainVao vao) {
    this->vao = vao;
}
void Chunk::setIBO(IBO ibo) {
    this->ibo = ibo;
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

LODChunk Chunk::getLodChunk(int elementId) {
    return lodChunks[elementId];
};

LODChunk* Chunk::getLodChunkPtr(int elementId) {
    return &lodChunks[elementId];
};

std::vector<LODChunk> Chunk::getLodChunks() {
    return lodChunks;
}

void Chunk::setPosition(glm::vec2 position) {
    this->position = position;
}

LODChunk::LODChunk(int lod) {
    this->lod = lod;
    hasMesh = false;
    hasRequestedMesh = false;
    this->Mesh = new Chunk();
}

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

Chunk* LODChunk::getChunk() {
    return Mesh;
}