#include <Aircraft.h>

Aircraft::Aircraft(const Characteristics& chars) : config(chars) {}

glm::vec3 Aircraft::update(glm::vec3 cameraPosition, glm::vec3 planeForward, float deltaTime, glm::vec3 planeUp, glm::vec3 planeRight) {
    // Constants (moved to class header in real implementation)
    const float wingArea = 24.0f;
    const float wingspan = 11.0f;
    const float mass = 2000.0f;
    const float maxThrust = 100000.0f;
    const float throttle = 0.8f;
    glm::vec3 windVelocity = glm::vec3(0.0f, 0.0f, -2.0f); // Example wind


    // Initialize
    if (firstUpdate) {
        velocity = planeForward * 55.0f;  // Start at stall speed
        currentPosition = cameraPosition;
        firstUpdate = false;
    }
    glm::vec3 trueAirspeed = velocity - windVelocity;

    // 1. Basic Forces (simplified like Jakob's approach)
    glm::vec3 weight = { 0, -mass * 9.81f, 0 };  // Constant weight
    glm::vec3 thrust = planeForward * (throttle * maxThrust);  // Simple thrust

    // 2. Aerodynamic Forces (using your improved functions)
    float airDensity = getAirDensityISA(currentPosition.y);
    float aspectRatio = (wingspan * wingspan) / wingArea;

    // Calculate angle of attack
    float aoa = calculateAngleOfAttack(planeForward, trueAirspeed, planeUp);
    aoa = glm::clamp(aoa, -5.0f, 15.0f);  // Realistic AoA limits

    // Lift parameters (matches website's linear CL model)
    float CL = 0.2f + 0.08f * aoa;  // CL_0 + CL_alpha * AoA
    CL = glm::clamp(CL, -0.3f, 1.2f);

    glm::vec3 lift = calculateLift(airDensity, trueAirspeed, wingArea, CL, planeUp);
    glm::vec3 drag = calculateDrag(airDensity, trueAirspeed, wingArea, CL, aspectRatio);

    weight = lift;

    // 3. Net Forces (remove artificial stability hacks)
    glm::vec3 netForce = lift + thrust - drag - weight;

    // 4. Physics Integration
    glm::vec3 acceleration = netForce / mass;
    velocity += acceleration * deltaTime;

    // 5. Basic Speed Limits (instead of artificial velocity clamping)
    float speed = glm::length(velocity);
    if (speed > 150.0f) {
        velocity = glm::normalize(velocity) * 150.0f;
    }
    if (speed < 55.0f) {
        velocity = glm::normalize(velocity) * 55.0f;
    }

    // 6. Update Position
    currentPosition += velocity * deltaTime;
    //currentPosition.y = glm::max(currentPosition.y, 50.0f);

    //std::cout << "AOA\t" << aoa << std::endl;
    //std::cout << "CurrentPosition" << "\t" << currentPosition.x << "\t" << currentPosition.y << "\t" << currentPosition.z << std::endl;
    //std::cout << "Plane     Front" << "\t" << planeForward.x << "\t" << planeForward.y << "\t" << planeForward.z << "\n" << std::endl;


    return currentPosition;


    //// Constants
    //const float wingArea = 24.0f;  // Area of the wings
    //const float wingspan = 11.0f;  // Wingspan of the plane
    //const float maxThrust = 100000.0f;  // Maximum thrust
    //const float throttle = 0.8f;  // Throttle setting
    //const float mass = 2000.0f;  // Mass of the plane
    //const float maxSpeed = 150.0f;  // Maximum speed
    //const float minSpeed = 55.0f;  // Minimum speed (stall speed)

    //if (firstUpdate) {
    //    velocity = planeForward * minSpeed;
    //    currentPosition = cameraPosition;
    //    firstUpdate = false;
    //}

    //// Forces
    //glm::vec3 weight = mass * glm::vec3(0.0f, -9.81f, 0.0f);  // Gravity
    //float airDensity = getAirDensityISA(currentPosition.y);
    //float speed = glm::length(velocity);
    //float q = 0.5f * airDensity * speed * speed;

    //// Normalize direction
    //glm::vec3 velocityDir = glm::normalize(velocity);
    //if (glm::length(velocityDir) < 0.001f) velocityDir = planeForward;

    //// Correct AoA calculation
    //float aoa = calculateAngleOfAttack(planeForward, velocity, planeUp);
    //aoa = glm::clamp(aoa, -5.0f, 10.0f);  // AoA limit for realistic flight

    //// Compute lift (adjust for realistic lift based on AoA)
    //float CL_0 = 0.2f;  // baseline lift at 0 AoA
    //float CL_alpha = 0.08f;  // per-degree lift increase (approx 4.8 per radian)
    //float CL = glm::clamp(CL_0 + CL_alpha * aoa, -0.3f, 1.2f);  // Lift calculation

    //// Lift direction calculation (perpendicular to velocity)
    //glm::vec3 liftDir = glm::normalize(glm::cross(glm::cross(velocityDir, planeUp), velocityDir));
    //if (glm::length(liftDir) < 0.001f) liftDir = planeUp;
    //glm::vec3 lift = liftDir * q * wingArea * CL;

    //// Ensure lift balances weight for level flight
    //if (glm::length(lift) < glm::length(weight)) {
    //    lift += (weight - lift);  // Adjust lift if it doesn't balance weight
    //}

    //// Calculate drag
    //float aspectRatio = (wingspan * wingspan) / wingArea;
    //glm::vec3 drag = calculateDrag(airDensity, velocity, wingArea, CL, aspectRatio);

    //// Calculate thrust
    //float mach = getMachNumber(glm::length(velocity), currentPosition.y);
    //glm::vec3 thrust = calculateThrust(maxThrust, throttle, mach, currentPosition.y, planeForward);

    //// Net force (Lift + Thrust - Drag - Weight)
    //glm::vec3 netForce = lift + thrust - drag - weight;

    //// Damping (lateral) to prevent roll and yaw instability
    //float rightSpeed = glm::dot(velocity, planeRight);
    //glm::vec3 rightDamping = -planeRight * rightSpeed * 15.0f;
    //netForce += rightDamping;

    //// Acceleration and velocity update
    //glm::vec3 acceleration = netForce / mass;
    //velocity += acceleration * deltaTime;

    //// Clamp speed (stall and max speed)
    //speed = glm::length(velocity);
    //const float stallSpeed = 55.0f;
    //if (speed > maxSpeed) {
    //    velocity *= glm::clamp(1.0f - (speed - maxSpeed) * deltaTime, 0.95f, 1.0f);
    //}
    //else if (speed < stallSpeed) {
    //    velocity += planeUp * 5.0f * deltaTime;
    //}

    //// Update position (make sure it doesn't go below a certain altitude)
    //glm::vec3 newPosition = currentPosition + velocity * deltaTime;
    //newPosition.y = glm::max(newPosition.y, 50.0f);  // Ensure altitude doesn't go below 50 meters
    //currentPosition = newPosition;

    //// Debugging output to track the position and forward vector
    //printf("Position: (%.1f, %.1f, %.1f)\n", currentPosition.x, currentPosition.y, currentPosition.z);
    //printf("Speed: %.1f m/s | AoA: %.1f° | Forward: (%.1f, %.1f, %.1f)\n", speed, aoa, planeForward.x, planeForward.y, planeForward.z);

    //return currentPosition;



    //// Constants
    //const float wingArea = 24.0f;
    //const float wingspan = 11.0f;
    //const float maxThrust = 100000.0f;
    //const float throttle = 0.8f;
    //const float mass = 2000.0f;
    //const float maxSpeed = 150.0f;
    //const float minSpeed = 55.0f;

    //if (firstUpdate) {
    //    velocity = planeForward * minSpeed;
    //    currentPosition = cameraPosition;
    //    firstUpdate = false;
    //}

    //// Forces
    //glm::vec3 weight = mass * glm::vec3(0.0f, -9.81f, 0.0f);
    //float airDensity = getAirDensityISA(currentPosition.y);
    //float speed = glm::length(velocity);
    //float q = 0.5f * airDensity * speed * speed;

    //// Normalize direction
    //glm::vec3 velocityDir = glm::normalize(velocity);
    //if (glm::length(velocityDir) < 0.001f) velocityDir = planeForward;

    //// Correct AoA calculation
    //float aoa = calculateAngleOfAttack(planeForward, velocity, planeUp);
    //aoa = glm::clamp(aoa, -5.0f, 10.0f);

    //// Compute lift only if AoA > 0

    //float CL_0 = 0.2f; // baseline lift at 0 AoA
    //float CL_alpha = 0.08f; // per-degree lift increase (approx 4.8 per radian)
    //float CL = glm::clamp(CL_0 + CL_alpha * aoa, -0.3f, 1.2f);

    //// Direction perpendicular to velocity and in the wing's "up" direction
    //glm::vec3 liftDir = glm::normalize(glm::cross(glm::cross(velocityDir, planeUp), velocityDir));
    //if (glm::length(liftDir) < 0.001f) liftDir = planeUp;

    //glm::vec3 lift = liftDir * q * wingArea * CL;

    //// Thrust force
    //glm::vec3 thrust = planeForward * (throttle * maxThrust);

    //// Drag
    //float aspectRatio = (wingspan * wingspan) / wingArea;
    //float CD = 0.02f + (0.4f * 0.4f) / (3.14159f * aspectRatio * 0.9f); // Assume avg CL for drag
    //glm::vec3 drag = -glm::normalize(velocity) * q * wingArea * CD;

    //// Net force
    //glm::vec3 netForce = lift + thrust + drag + weight;

    //// Damping (lateral)
    //float rightSpeed = glm::dot(velocity, planeRight);
    //glm::vec3 rightDamping = -planeRight * rightSpeed * 15.0f;
    //netForce += rightDamping;

    //glm::vec3 acceleration = netForce / mass;
    //velocity += acceleration * deltaTime;

    //// Clamp speed
    //speed = glm::length(velocity);
    //const float stallSpeed = 55.0f;
    //if (speed > maxSpeed) {
    //    velocity *= glm::clamp(1.0f - (speed - maxSpeed) * deltaTime, 0.95f, 1.0f);
    //}
    //else if (speed < stallSpeed) {
    //    velocity += planeUp * 5.0f * deltaTime;
    //}

    //// Update position
    //glm::vec3 newPosition = currentPosition + velocity * deltaTime;
    //newPosition.y = glm::max(newPosition.y, 50.0f);
    //currentPosition = newPosition;

    //// Debug
    //printf("POS: (%.1f, %.1f, %.1f) | SPD: %.1f m/s\n",
    //    newPosition.x, newPosition.y, newPosition.z,
    //    glm::length(velocity));
    //printf("AoA: %.1f° | Vertical Speed: %.1f m/s\n", aoa, velocity.y);

    //return currentPosition;

}

float Aircraft::calculateAngleOfAttack(glm::vec3 planeForward, glm::vec3 relativeVelocity, glm::vec3 planeUp) {
    glm::vec3 velocityDir = glm::normalize(relativeVelocity);
    planeForward = glm::normalize(planeForward); // Ensure normalization

    // Calculate angle between velocity and aircraft's forward direction
    float dot = glm::clamp(glm::dot(velocityDir, planeForward), -1.0f, 1.0f);
    float angleRad = acos(dot);

    // Determine sign using aircraft's up direction
    float sign = (glm::dot(planeUp, velocityDir) > 0.0f) ? 1.0f : -1.0f;
    return glm::degrees(angleRad) * sign;
}

float Aircraft::getMachNumber(float speed, float altitude) {
    const float SPEED_OF_SOUND_SEA_LEVEL = 343.0f; // m/s  
    // Speed of sound decreases with altitude (~-0.6% per 1,000m)  
    float speedOfSound = SPEED_OF_SOUND_SEA_LEVEL * (1.0f - 0.006f * (altitude / 1000.0f));
    return speed / speedOfSound;
}

glm::vec3 Aircraft::calculateLift(
    float airDensity,
    glm::vec3 velocity,
    float wingArea,
    float liftCoefficient,
    const glm::vec3& planeUp    // Aircraft's upward direction in world space (from orientation)
) {
    float speedSquared = glm::dot(velocity, velocity);
    if (speedSquared < 1e-6f) return glm::vec3(0.0f);

    // Lift acts in the aircraft's UP direction (body frame)
    glm::vec3 liftDir = glm::normalize(planeUp);
    float liftMagnitude = 0.5f * airDensity * speedSquared * wingArea * liftCoefficient;
    return liftDir * liftMagnitude;
}

glm::vec3 Aircraft::calculateDrag(
    float airDensity,
    glm::vec3 velocity,
    float wingArea,
    float CL,               // from AoA
    float aspectRatio       // wingspan² / area
) {
    float speedSquared = glm::dot(velocity, velocity);
    if (speedSquared < 1e-6f) {
        return glm::vec3(0.0f);
    }

    // Drag coefficients
    float CD0 = 0.02f;      // Adjust based on aircraft geometry
    float e = 0.8f;         // Typical value for modern aircraft (0.7–0.9)
    float k = 1.0f / (glm::pi<float>() * aspectRatio * e);
    float CD = CD0 + k * CL * CL;

    float dragMagnitude = 0.5f * airDensity * speedSquared * wingArea * CD;
    glm::vec3 dragDir = -glm::normalize(velocity);
    return dragDir * dragMagnitude;
}

glm::vec3 Aircraft::calculateThrust(
    float maxThrust,      // Max static thrust (at sea level, zero speed)
    float throttle,       // 0.0–1.0
    float mach,           // Current Mach number
    float altitude,       // Meters
    glm::vec3 planeForward // Thrust direction
) {
    // 1. Altitude loss (approximate)
    float altitudeKm = altitude / 1000.0f;
    float altitudeFactor = glm::clamp(1.0f - 0.025f * altitudeKm, 0.0f, 1.0f);

    // 2. Mach number effect (thrust drops near Mach 1)
    float machFactor = 1.0f;
    if (mach > 0.9f) {
        machFactor = glm::clamp(1.0f - (mach - 0.9f) * 2.0f, 0.2f, 1.0f);
    }

    // 3. Airspeed effect (ram drag for jets)
    // Assume thrust decays linearly with mach (simplified)
    float speedFactor = 1.0f - 0.2f * mach;

    // 4. Combine factors
    float thrustMagnitude = maxThrust * throttle
        * altitudeFactor
        * machFactor
        * speedFactor;

    return planeForward * thrustMagnitude;
}

float Aircraft::getAirDensityISA(float altitude) {
    const float T0 = 288.15f;    // Sea-level temp (15°C in Kelvin)
    const float P0 = 101325.0f;  // Sea-level pressure (Pa)
    const float L = 0.0065f;     // Temp lapse rate (K/m)
    const float R = 287.05f;     // Specific gas constant for air (J/kg·K)
    const float g = 9.80665f;    // Gravity (m/s²)

    if (altitude <= 11000.0f) {  // Troposphere (0-11 km)
        float T = T0 - L * altitude;
        float P = P0 * powf(T / T0, g / (L * R));
        return P / (R * T);
    }
    else {                     // Stratosphere (11+ km, temp constant)
        float T11 = 216.65f;     // Temp at 11 km (K)
        float P11 = 22632.0f;    // Pressure at 11 km (Pa)
        float exponent = -g * (altitude - 11000.0f) / (R * T11);
        float P = P11 * expf(exponent);
        return P / (R * T11);
    }
}

Aircraft::~Aircraft() {
}