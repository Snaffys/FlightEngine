#ifndef PARTICLE_H
#define PARTICLE_H

#include "glm/glm.hpp"
#include "memory"

struct Particle {
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 colorStart;
	glm::vec4 colorEnd;
	float sizeStart;
	float sizeEnd;

	float life;  // Normalized [0.0, 1.0]
	float rotation;

	// Custom Data (Not sent to GPU)
	std::unique_ptr<void, void(*)(void*)> custom_data{ nullptr, [](void*) {} };

	float lifetime;
	template<typename T> T* get_custom() {
		return static_cast<T*>(custom_data.get());
	}
};

#endif