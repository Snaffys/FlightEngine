#include "ParticleManager/ParticleGenerator.h"
ParticleGenerator::ParticleGenerator(unsigned int amount, Config config) 
    : amount(amount), config(config){

    // Initialize quad VBO (billboard geometry)
    quadVBO = {
        // Positions          // UVs
        { -0.5f, -0.5f, 0.0f,  0.0f, 0.0f }, // Bottom-left
        {  0.5f, -0.5f, 0.0f,  1.0f, 0.0f }, // Bottom-right
        { -0.5f,  0.5f, 0.0f,  0.0f, 1.0f }, // Top-left
        {  0.5f,  0.5f, 0.0f,  1.0f, 1.0f }  // Top-right
    };

    particles.resize(amount);
    glm::vec3 rainArea(100.0f, 50.0f, 100.0f); // 100x50x100 unit area

    for (auto& p : particles) {
        respawnParticle(p, glm::vec3(0), glm::vec3(100, 50, 100));
    }

    // Create instance VBO and VAO
    instanceVBO = VBO(particles);
    vao = ParticleVAO(quadVBO, instanceVBO);

    //// Load texture if provided
    //if (texPath) {
    //    texture = Texture(texPath);
    //}

    shader = Shader("shaders/vertex/particles.vert", "shaders/fragment/particles.frag");
};

void ParticleGenerator::Update(float dt, unsigned int nrNewParticles, glm::vec3 areaSize, glm::vec3 emitterPos) {
    bool dataChanged = false;
    // 1. Update existing particles
    for (auto& p : particles) {
        if (p.life > 0.0f) {
            // Apply custom behavior
            if (config.behaviorUpdate) {
                config.behaviorUpdate(p, dt);
            }

            // Default physics
            p.position += p.velocity * dt;
            p.life -= dt / p.lifetime;

            dataChanged = true;
        }
    }

    // 2. Spawn new particles
    for (unsigned i = 0; i < nrNewParticles; ++i) {
        unsigned idx = firstUnusedParticle();
        glm::vec3 offset = {
            (rand() % 100 / 100.0f - 0.5f) * areaSize.x,
            (rand() % 100 / 100.0f - 0.5f) * areaSize.y,
            (rand() % 100 / 100.0f - 0.5f) * areaSize.z
        };
        respawnParticle(particles[idx], emitterPos, offset);
        dataChanged = true;
    }

    if (dataChanged) {
        // Update GPU buffer
        instanceVBO.Bind();
        glBufferSubData(GL_ARRAY_BUFFER, 0,
            particles.size() * offsetof(Particle, custom_data),
            particles.data());
        //glBufferData(GL_ARRAY_BUFFER, particles.size() * offsetof(Particle, custom_data), particles.data(), GL_DYNAMIC_DRAW);
    }
}

void ParticleGenerator::Draw(glm::mat4 view, glm::mat4 projection, glm::vec3 cameraUp, glm::vec3 cameraRight) {
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // 1. Activate shader
    shader.Activate();

    // 2. Set required matrices
    shader.setMat4("uView", view);
    shader.setMat4("uProjection", projection);

    shader.setVec3("uCameraRight", cameraRight);
    shader.setVec3("uCameraUp", cameraUp);

    static float globalWind = 0.5f; // Example wind source
    // 3. Set time-dependent uniforms
    shader.setFloat("uTime", glfwGetTime());
    shader.setFloat("uWindStrength", globalWind);


    vao.Bind();
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<GLsizei>(particles.size()));
    vao.Unbind();

    //glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
}

void ParticleGenerator::respawnParticle(Particle& particle, glm::vec3 center, glm::vec3 areaSize) {
    // Position with offset
    particle.position = center + areaSize;

    // Velocity with randomness
    particle.velocity = config.baseVelocity + glm::vec3(
        (rand() % 100 / 100.0f - 0.5f) * config.velocityRandomness.x,
        (rand() % 100 / 100.0f - 0.5f) * config.velocityRandomness.y,
        (rand() % 100 / 100.0f - 0.5f) * config.velocityRandomness.z
    );

    // Visual properties
    particle.colorStart = config.startColor;
    particle.colorEnd = config.endColor;
    particle.sizeStart = config.sizeStart;
    particle.sizeEnd = config.sizeEnd;
    particle.lifetime = config.minLifetime +
        (rand() % 1000) / 1000.0f * (config.maxLifetime - config.minLifetime);
    particle.life = 1.0f;
    particle.rotation = 0.0f;

    // Initialize custom data if behavior exists
    if (config.behaviorInit) {
        config.behaviorInit(particle);
    }
}

unsigned int ParticleGenerator::firstUnusedParticle() {
    // Search from last used to end
    for (unsigned int i = lastUsedParticle; i < amount; ++i) {
        if (particles[i].life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }

    // Wrap-around search from beginning
    for (unsigned int i = 0; i < lastUsedParticle; ++i) {
        if (particles[i].life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }

    // All particles are alive, override first one
    lastUsedParticle = 0;
    return 0;
}