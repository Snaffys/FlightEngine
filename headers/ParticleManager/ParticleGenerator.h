#ifndef PARTICLEGENERATOR_H
#define PARTICLEGENERATOR_H

#include "iostream"
#include "glm/glm.hpp"
//#include "Buffers/VBO.h"
//#include "Buffers/VAO.h"
#include "Model/Mesh.h"



class ParticleGenerator {
public:
	struct Config {
		glm::vec3 baseVelocity = { 0,-5,0 };
		glm::vec3 velocityRandomness = { 1,0,0 };
		glm::vec4 startColor = { 0.7f,0.8f,1.0f,1.0f };
		glm::vec4 endColor = { 0.7f,0.8f,1.0f,0.0f };
		float sizeStart = 0.1f;
		float sizeEnd = 0.2f;
		float minLifetime = 1.5f;
		float maxLifetime = 3.0f;

		std::function<void(Particle&)> behaviorInit = nullptr;
		std::function<void(Particle&, float dt)> behaviorUpdate = nullptr;
	};
	ParticleGenerator() = default;
	ParticleGenerator(unsigned int amount, Config config = {});
	//ParticleGenerator(unsigned int amount, const char* texPath);
	void Update(float dt, unsigned int nrNewParticles, glm::vec3 areaSize, glm::vec3 emitterPos);
	void Draw(glm::mat4 view, glm::mat4 projection, glm::vec3 cameraUp, glm::vec3 cameraRight);

	template<typename T>
	void SetBehavior(std::function<void(Particle&, T&, float)> behavior) {
		behaviors[typeid(T).hash_code()] = [behavior](Particle& p, float dt) {
			// Initialize custom data if needed
			if (!p.custom_data) {
				p.custom_data = std::unique_ptr<void, void(*)(void*)>(
					new T(),
					[](void* ptr) { delete static_cast<T*>(ptr); }
				);
			}
			behavior(p, *static_cast<T*>(p.custom_data.get()), dt);
			};
	}


private:
	VBO quadVBO;
	VBO instanceVBO;
	ParticleVAO vao;
	std::vector<Particle> particles;
	unsigned int amount;
	unsigned int lastUsedParticle = 0;
	unsigned int firstUnusedParticle();
	void respawnParticle(Particle& p, glm::vec3 position, glm::vec3 offset);
	Shader shader;
	Texture texture;
	Config config;
	std::unordered_map<size_t, std::function<void(Particle&, float)>> behaviors;

};

#endif