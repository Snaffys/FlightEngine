#ifndef AUDIO_H
#define AUDIO_H

#include <openAL/al.h>
#include <openAL/alc.h>
#include <iostream>

class Audio {
public:
	Audio(const std::string& filename);

	~Audio();
private:
	std::string getFileExtension(const std::string& filename);
	ALuint loadSoundFile(const std::string& filename);
};

#endif