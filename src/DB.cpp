#include <db.h>
#include "terrain/terrain.h" 

#include <iostream>
#include <cstring>

#include "db.h"
#include <iostream>
#include <filesystem>
//
//DB::DB() : env(nullptr){
//    //// Create environment
//    //if (mdb_env_create(&env)) { std::cerr << "Failed to create LMDB env\n"; env = nullptr; return; }
//    //std::filesystem::create_directories("res/database");
//    //mdb_env_set_mapsize(env, 200ULL * 2024 * 2024);
//    //if (mdb_env_open(env, "res/database", 0, 0664)) { std::cerr << "Failed to open LMDB env\n"; mdb_env_close(env); env = nullptr; }
//    std::string dbPath = "res/database";
//
//    // Delete existing database directory if it exists
//    //if (std::filesystem::exists(dbPath)) {
//    //    std::error_code ec;
//    //    std::filesystem::remove_all(dbPath, ec);
//    //    if (ec) {
//    //        std::cerr << "Failed to delete database: " << ec.message() << "\n";
//    //        return;
//    //    }
//    //}
//
//    // Recreate directory
//    std::filesystem::create_directories(dbPath);
//
//    // Create environment
//    if (mdb_env_create(&env)) {
//        std::cerr << "Failed to create LMDB env\n";
//        env = nullptr;
//        return;
//    }
//
//    // Set mapsize (fixed 200MB calculation)
//    mdb_env_set_mapsize(env, 200ULL * 1024 * 1024);
//
//    // Open environment
//    if (mdb_env_open(env, dbPath.c_str(), 0, 0664)) {
//        std::cerr << "Failed to open LMDB env\n";
//        mdb_env_close(env);
//        env = nullptr;
//    }
//
//
//
//    //std::string dbPath = "res/database";
//
//    //// Delete existing database ONLY if you want to reset it
//    ///* if (std::filesystem::exists(dbPath)) {
//    //    ...
//    //} */
//
//    //int rc = mdb_env_create(&env);
//    //if (rc != MDB_SUCCESS) {
//    //    std::cerr << "Failed to create LMDB env: " << mdb_strerror(rc) << "\n";
//    //    return;
//    //}
//
//    //rc = mdb_env_set_mapsize(env, 200ULL * 1024 * 1024); // 200MB
//    //if (rc != MDB_SUCCESS) {
//    //    std::cerr << "Failed to set mapsize: " << mdb_strerror(rc) << "\n";
//    //    mdb_env_close(env);
//    //    env = nullptr;
//    //    return;
//    //}
//
//    //rc = mdb_env_open(env, dbPath.c_str(), MDB_NOSUBDIR, 0664);
//    //if (rc != MDB_SUCCESS) {
//    //    std::cerr << "Failed to open LMDB env: " << mdb_strerror(rc) << "\n";
//    //    mdb_env_close(env);
//    //    env = nullptr;
//    //}
//}
//
//DB::~DB() {
//    if (env) mdb_env_close(env);
//}
//
//
//
//// DB.cpp
//void DB::storeHeightMap(const glm::vec2& position, unsigned int octaveCount, float scalingBias,
//    unsigned int vertexWidth, unsigned int vertexHeight,
//    const float* data, size_t count) {
//    if (!env) return;
//
//    MDB_txn* txn;
//    MDB_dbi dbi;
//    mdb_txn_begin(env, nullptr, 0, &txn);
//    mdb_dbi_open(txn, nullptr, 0, &dbi);
//
//    ChunkKey key{ position, octaveCount, scalingBias, vertexWidth, vertexHeight };
//    MDB_val mdbKey{ sizeof(ChunkKey), &key };
//    MDB_val mdbData{ count * sizeof(float), const_cast<float*>(data) };
//
//    mdb_put(txn, dbi, &mdbKey, &mdbData, 0);
//    mdb_txn_commit(txn);
//    mdb_dbi_close(env, dbi);
//}
//
//bool DB::loadHeightMap(const glm::vec2& position, unsigned int octaveCount, float scalingBias,
//    unsigned int vertexWidth, unsigned int vertexHeight,
//    std::vector<float>& out) {
//    if (!env) return false;
//
//    MDB_txn* txn;
//    MDB_dbi dbi;
//    mdb_txn_begin(env, nullptr, MDB_RDONLY, &txn);
//    mdb_dbi_open(txn, nullptr, 0, &dbi);
//
//    ChunkKey key{ position, octaveCount, scalingBias, vertexWidth, vertexHeight };
//    MDB_val mdbKey{ sizeof(ChunkKey), &key }, data;
//
//    int rc = mdb_get(txn, dbi, &mdbKey, &data);
//    if (rc == MDB_NOTFOUND) {
//        mdb_txn_abort(txn);
//        mdb_dbi_close(env, dbi);
//        return false;
//    }
//    if (rc) {
//        mdb_txn_abort(txn);
//        mdb_dbi_close(env, dbi);
//        return false;
//    }
//
//    size_t count = data.mv_size / sizeof(float);
//    out.resize(count);
//    std::memcpy(out.data(), data.mv_data, data.mv_size);
//
//    mdb_txn_commit(txn);
//    mdb_dbi_close(env, dbi);
//    return true;
//}
//
//void DB::debugPrintAllChunks() {
//    if (!env) return;
//
//    MDB_txn* txn;
//    MDB_dbi dbi;
//    MDB_cursor* cursor;
//
//    mdb_txn_begin(env, nullptr, MDB_RDONLY, &txn);
//    mdb_dbi_open(txn, nullptr, 0, &dbi);
//    mdb_cursor_open(txn, dbi, &cursor);
//
//    MDB_val key, data;
//    while (mdb_cursor_get(cursor, &key, &data, MDB_NEXT) == 0) {
//        // Decode the key
//        ChunkKey* chunkKey = reinterpret_cast<ChunkKey*>(key.mv_data);
//        std::cout << "=== Chunk Key ===" << "\n"
//            << "Position: (" << chunkKey->position.x << ", " << chunkKey->position.y << ")\n"
//            << "Octaves: " << chunkKey->octaveCount << "\n"
//            << "Scaling Bias: " << chunkKey->scalingBias << "\n"
//            << "Vertex Width: " << chunkKey->vertexWidth << "\n"
//            << "Vertex Height: " << chunkKey->vertexHeight << "\n";
//
//        // Decode the heightmap data
//        float* heightData = reinterpret_cast<float*>(data.mv_data);
//        size_t dataCount = data.mv_size / sizeof(float);
//
//        std::cout << "Heightmap Data (" << dataCount << " values):\n";
//
//        // Print first 5 values as a sample
//        std::cout << "Sample [First 5 Values]: ";
//        for (size_t i = 0; i < std::min(dataCount, static_cast<size_t>(5)); i++) {
//            std::cout << heightData[i] << " ";
//        }
//        std::cout << "\n\n";
//    }
//
//    mdb_cursor_close(cursor);
//    mdb_txn_abort(txn);
//    mdb_dbi_close(env, dbi);
//}
//
//
//
//
////Vec2Key::Vec2Key(const glm::vec2& pos) : position(pos) {}
////
////MDB_val Vec2Key::to_mdb_key() const {
////    return MDB_val{ sizeof(glm::vec2), (void*)&position };
////}
////
////glm::vec2 Vec2Key::from_mdb_key(const MDB_val& key) {
////    return *(glm::vec2*)key.mv_data;
////}
////
////DB::DB() {
////    MDB_dbi dbi;
////    MDB_val key, data;
////    MDB_txn* txn;
////    int rc;
////
////    // Create the LMDB environment
////    rc = mdb_env_create(&env);
////    if (rc) {
////        std::cerr << "Failed to create LMDB environment: " << mdb_strerror(rc) << std::endl;
////        return;
////    }
////
////    // Set the maximum size of the database (10 MB in this case)
////    rc = mdb_env_set_mapsize(env, 10 * 1024 * 1024);
////    if (rc) {
////        std::cerr << "Failed to set mapsize: " << mdb_strerror(rc) << std::endl;
////        mdb_env_close(env);
////        return;
////    }
////
////    // Open the environment
////    rc = mdb_env_open(env, "res/database", 0, 0664);
////    if (rc) {
////        std::cerr << "Failed to open LMDB environment: " << mdb_strerror(rc) << std::endl;
////        mdb_env_close(env);
////        return;
////    }
////}
////
////void DB::storeChunkData(glm::vec2 position) {
////    MDB_txn* txn;
////    MDB_dbi dbi;
////    MDB_val mdb_key, mdb_data;
////
////    // Open transaction
////    mdb_txn_begin(env, nullptr, 0, &txn);
////    mdb_dbi_open(txn, nullptr, 0, &dbi);
////
////    // Serialize key and data
////    Vec2Key vecKey(position);
////    mdb_key = vecKey.to_mdb_key();
////
////    mdb_data.mv_size = sizeof(position.x) - 1; // Value size (excluding null terminator)
////    mdb_data.mv_data = &position.x;  // Value data
////    //std::vector<uint8_t> serializedChunk = chunk.serialize();
////    //mdb_data.mv_size = serializedChunk.size();
////    //mdb_data.mv_data = serializedChunk.data();
////
////    // Put data into database
////    int res = mdb_put(txn, dbi, &mdb_key, &mdb_data, 0);
////    if (res != 0) {
////        std::cerr << "Failed to store chunk: " << mdb_strerror(res) << std::endl;
////    }
////
////    // Commit transaction
////    mdb_txn_commit(txn);
////    mdb_dbi_close(env, dbi);
////};
////
////void DB::loadChunkData(glm::vec2 position) {
////    MDB_txn* txn;
////    MDB_dbi dbi;
////    MDB_val mdb_key, mdb_data;
////
////    // Open transaction
////    mdb_txn_begin(env, nullptr, MDB_RDONLY, &txn);
////    mdb_dbi_open(txn, nullptr, 0, &dbi);
////
////    // Serialize key and data
////    Vec2Key vecKey(position);
////    mdb_key = vecKey.to_mdb_key();
////
////    if (mdb_get(txn, dbi, &mdb_key, &mdb_data) == 0) {
////        //std::vector<uint8_t> buffer((uint8_t*)mdb_data.mv_data, (uint8_t*)mdb_data.mv_data + mdb_data.mv_size);
////        
////        //std::cout << *(float*)mdb_data.mv_data << std::endl;
////        
////        //return Chunk::deserialize(buffer);
////    }
////    else {
////        std::cerr << "Chunk not found" << std::endl;
////        //return {};
////    }
////
////    mdb_txn_abort(txn);
////    mdb_dbi_close(env, dbi);
////}
////
////DB::~DB() {
////}



DB::DB() : env(nullptr), dbi(0) {
    std::string dbPath = "res/database";

    // Create directory if it doesn't exist
    std::filesystem::create_directories(dbPath);

    // Initialize LMDB environment
    if (mdb_env_create(&env) != MDB_SUCCESS) {
        std::cerr << "Failed to create LMDB environment\n";
        return;
    }

    // Set map size to 200MB
    if (mdb_env_set_mapsize(env, 200ULL * 1024 * 1024) != MDB_SUCCESS) {
        std::cerr << "Failed to set mapsize\n";
        mdb_env_close(env);
        env = nullptr;
        return;
    }

    // Open environment
    if (mdb_env_open(env, dbPath.c_str(), 0, 0664) != MDB_SUCCESS) {
        std::cerr << "Failed to open LMDB environment\n";
        mdb_env_close(env);
        env = nullptr;
        return;
    }

    // Open the DBI once
    MDB_txn* txn;
    if (mdb_txn_begin(env, nullptr, 0, &txn) != MDB_SUCCESS) {
        std::cerr << "Failed to create transaction for DBI open\n";
        mdb_env_close(env);
        env = nullptr;
        return;
    }

    if (mdb_dbi_open(txn, nullptr, 0, &dbi) != MDB_SUCCESS) {
        std::cerr << "Failed to open DBI\n";
        mdb_txn_abort(txn);
        mdb_env_close(env);
        env = nullptr;
        return;
    }

    if (mdb_txn_commit(txn) != MDB_SUCCESS) {
        std::cerr << "Failed to commit DBI open transaction\n";
        mdb_env_close(env);
        env = nullptr;
    }
}

DB::~DB() {
    if (env) {
        mdb_dbi_close(env, dbi); // Close DBI first
        mdb_env_close(env);
    }
}

void DB::storeHeightMap(const glm::vec2& position, unsigned int octaveCount, float scalingBias,
    unsigned int vertexWidth, unsigned int vertexHeight,
    const float* data, size_t count) {
    if (!env) return;

    MDB_txn* txn;
    if (mdb_txn_begin(env, nullptr, 0, &txn) != MDB_SUCCESS) {
        std::cerr << "Failed to start write transaction\n";
        return;
    }

    ChunkKey key{ position, octaveCount, scalingBias, vertexWidth, vertexHeight };
    MDB_val mdbKey{ sizeof(ChunkKey), &key };
    MDB_val mdbData{ count * sizeof(float), const_cast<float*>(data) };

    if (mdb_put(txn, dbi, &mdbKey, &mdbData, 0) != MDB_SUCCESS) {
        std::cerr << "Failed to store data\n";
        mdb_txn_abort(txn);
        return;
    }

    if (mdb_txn_commit(txn) != MDB_SUCCESS) {
        std::cerr << "Commit failed\n";
    }
}

bool DB::loadHeightMap(const glm::vec2& position, unsigned int octaveCount, float scalingBias,
    unsigned int vertexWidth, unsigned int vertexHeight,
    std::vector<float>& out) {
    if (!env) return false;

    MDB_txn* txn;
    if (mdb_txn_begin(env, nullptr, MDB_RDONLY, &txn) != MDB_SUCCESS) {
        std::cerr << "Failed to start read transaction\n";
        return false;
    }

    ChunkKey key{ position, octaveCount, scalingBias, vertexWidth, vertexHeight };
    MDB_val mdbKey{ sizeof(ChunkKey), &key }, mdbData;

    int rc = mdb_get(txn, dbi, &mdbKey, &mdbData);
    if (rc == MDB_NOTFOUND) {
        mdb_txn_abort(txn);
        return false;
    }
    else if (rc != MDB_SUCCESS) {
        std::cerr << "Error reading data: " << mdb_strerror(rc) << "\n";
        mdb_txn_abort(txn);
        return false;
    }

    out.resize(mdbData.mv_size / sizeof(float));
    memcpy(out.data(), mdbData.mv_data, mdbData.mv_size);

    mdb_txn_abort(txn); // Abort read-only transaction
    return true;
}

void DB::debugPrintAllChunks() {
    if (!env) return;

    MDB_txn* txn;
    MDB_cursor* cursor;

    if (mdb_txn_begin(env, nullptr, MDB_RDONLY, &txn) != MDB_SUCCESS) {
        std::cerr << "Failed to start read transaction\n";
        return;
    }

    if (mdb_cursor_open(txn, dbi, &cursor) != MDB_SUCCESS) {
        std::cerr << "Failed to open cursor\n";
        mdb_txn_abort(txn);
        return;
    }

        MDB_val key, data;
        while (mdb_cursor_get(cursor, &key, &data, MDB_NEXT) == 0) {
            // Decode the key
            ChunkKey* chunkKey = reinterpret_cast<ChunkKey*>(key.mv_data);
            std::cout << "=== Chunk Key ===" << "\n"
                << "Position: (" << chunkKey->position.x << ", " << chunkKey->position.y << ")\n"
                << "Octaves: " << chunkKey->octaveCount << "\n"
                << "Scaling Bias: " << chunkKey->scalingBias << "\n"
                << "Vertex Width: " << chunkKey->vertexWidth << "\n"
                << "Vertex Height: " << chunkKey->vertexHeight << "\n";
    
            // Decode the heightmap data
            float* heightData = reinterpret_cast<float*>(data.mv_data);
            size_t dataCount = data.mv_size / sizeof(float);
    
            std::cout << "Heightmap Data (" << dataCount << " values):\n";
    
            // Print first 5 values as a sample
            std::cout << "Sample [First 5 Values]: ";
            for (size_t i = 0; i < std::min(dataCount, static_cast<size_t>(5)); i++) {
                std::cout << heightData[i] << " ";
            }
            std::cout << "\n\n";
        }

    mdb_cursor_close(cursor);
    mdb_txn_abort(txn);
}