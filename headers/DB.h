#ifndef DB_H
#define DB_H

//#include <iostream>
//#include <lmdb/lmdb.h>
//#include "glm/glm.hpp"
////#include "Terrain.h"
//
//struct Vec2Key {
//    glm::vec2 position;
//
//    Vec2Key(const glm::vec2& pos);
//
//    MDB_val to_mdb_key() const;
//
//    glm::vec2 from_mdb_key(const MDB_val& key);
//};
//
//class DB {
//public:
//	DB();
//    void storeChunkData(glm::vec2 position);
//    void loadChunkData(glm::vec2 position);
//    //std::vector<char> serializeChunk(Chunk& chunk);
//
//
//	~DB();
//private:
//	MDB_env* env;
//};



#include <lmdb/lmdb.h>
#include <vector>
#include <glm/glm.hpp>

//class DB {
//public:
//    DB();
//    ~DB();
//
//    void storeHeightMap(const glm::vec2& position, int lod, const float* data, size_t count);
//    bool loadHeightMap(const glm::vec2& position, int lod, std::vector<float>& out);
//
//    // DB.cpp
//    void storeHeightMap(const glm::vec2& position, unsigned int octaveCount, float scalingBias,
//        unsigned int vertexWidth, unsigned int vertexHeight,
//        const float* data, size_t count);
//
//    bool loadHeightMap(const glm::vec2& position, unsigned int octaveCount, float scalingBias,
//        unsigned int vertexWidth, unsigned int vertexHeight,
//        std::vector<float>& out);
//
//    void debugPrintAllChunks();
//
//
//
//private:
//    MDB_env* env = nullptr;
//
//
//        struct ChunkKey {
//            glm::vec2 position;
//            unsigned int octaveCount;
//            float scalingBias;
//            unsigned int vertexWidth;
//            unsigned int vertexHeight;
//        };
//
//};


struct ChunkKey {
    glm::vec2 position;
    unsigned int octaveCount;
    float scalingBias;
    unsigned int vertexWidth;
    unsigned int vertexHeight;
};

class DB {
private:
    MDB_env* env;
    MDB_dbi dbi; // Add DBI as a member

public:
    DB();
    ~DB();

    void storeHeightMap(const glm::vec2& position, unsigned int octaveCount, float scalingBias,
        unsigned int vertexWidth, unsigned int vertexHeight,
        const float* data, size_t count);

    bool loadHeightMap(const glm::vec2& position, unsigned int octaveCount, float scalingBias,
        unsigned int vertexWidth, unsigned int vertexHeight,
        std::vector<float>& out);

    void debugPrintAllChunks();
};


#endif