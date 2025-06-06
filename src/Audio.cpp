//#include "Audio.h"
//
//Audio::Audio(const std::string& filename) {
//    // OpenAL device and context
//    ALCdevice* device = alcOpenDevice(nullptr);
//    if (!device) {
//        std::cerr << "Failed to open OpenAL device!" << std::endl;
//        return;
//    }
//
//    ALCcontext* context = alcCreateContext(device, nullptr);
//    if (!context) {
//        std::cerr << "Failed to create OpenAL context!" << std::endl;
//        return;
//    }
//
//    alcMakeContextCurrent(context);
//
//    loadSoundFile(filename);
//}
//
//ALuint Audio::loadSoundFile(const std::string& filename) {
//    std::string ext = getFileExtension(filename);
//    if (ext == "wav" || ext == "flac")
//        return loadWavFlacFile(filename.c_str());
//    else if (ext == "ogg")
//        return loadOggFile(filename.c_str());
//    else if (ext == "mp3")
//        return loadMp3File(filename.c_str());
//    else {
//        std::cerr << "Unsupported file format: " << filename << std::endl;
//        return 0;
//    }
//}
//
//std::string Audio::getFileExtension(const std::string& filename) {
//    size_t dot = filename.find_last_of(".");
//    if (dot == std::string::npos) 
//        return "";
//    return 
//        filename.substr(dot + 1);
//}