#ifndef Aircraft_H
#define Aircraft_H

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Characteristics {
    glm::vec3 velocity;
    float mass;
    float thrustForce;
    float dragCoefficient;
    float liftCoefficient;
    glm::vec3 gravity;

    Characteristics() = default;

    Characteristics(glm::vec3 velocity, float mass, float thrustForce,
        float dragCoefficient, float liftCoefficient,
        glm::vec3 gravity = glm::vec3(0, -9.80665f, 0))
        : velocity(velocity), mass(mass), thrustForce(thrustForce),
        dragCoefficient(dragCoefficient), liftCoefficient(liftCoefficient),
        gravity(gravity) {};
};

class Aircraft {
public:
    Aircraft() = default;
    Aircraft(const Characteristics& chars);

    //void update(float deltaTime, const glm::vec3& controlInputs);
    //float update();
    glm::vec3 update(glm::vec3 cameraPosition, glm::vec3 planeForward, float deltaTime, glm::vec3 planeUp, glm::vec3 planeRight);

    // Getters for rendering system
    glm::vec3& getPosition();
    glm::vec3& getRotation();

    glm::vec3 previousPostion = glm::vec3(0.0f);

    ~Aircraft();

private:
    bool firstUpdate = true;
    glm::vec3 velocity;  // Remove 'static'!
    glm::vec3 position;
    //glm::vec3 velocity{ 0.0f, 0.0f, -100.0f };  // Member variable instead of static
    //glm::vec3 position{ 0.0f };                 // Track position internally
    void calculateForces(float dt, const glm::vec3& controls);
    void integrate(float dt);

    glm::mat4 calculateRotationMatrix();

    float getAirDensityISA(float altitude);


    glm::vec3 calculateDrag(
        float airDensity,
        glm::vec3 velocity,
        float wingArea,
        float CL,               // from AoA
        float aspectRatio       // wingspan² / area
    );

    glm::vec3 calculateLift(
        float airDensity,
        glm::vec3 velocity,
        float wingArea,
        float liftCoefficient,
        const glm::vec3& planeUp    // Aircraft's upward direction in world space (from orientation)
    );

    glm::vec3 calculateThrust(
        float maxThrust,
        float throttle,
        float mach,
        float altitude,
        glm::vec3 planeForward
    );

    glm::vec3 currentPosition;


    float calculateAngleOfAttack(glm::vec3 planeForward, glm::vec3 relativeVelocity, glm::vec3 planeUp);
    float getMachNumber(float speed, float altitude);
    Characteristics config;
    bool isFirstFrame = true;

    float speed;

    //glm::vec3 position;
    //glm::vec3 velocity;
    //glm::vec3 rotation;      // pitch, yaw, roll (degrees)
    //glm::vec3 angularVelocity;
    //float currentThrust = 0;
};

#endif