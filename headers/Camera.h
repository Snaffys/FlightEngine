#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera {
public:
	Camera() = default;

	Camera(glm::vec3 cameraPosition, glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f),
		float movementSpeed = 3.5f, float mouseSensitivity = 0.1f, float actualSpeed = 0.0f);

	glm::mat4 getViewMatrix();

	void ProcessKeyPress(CameraMovement, float);
	void ProcessKeyPress(CameraMovement direction, float delta_time, glm::vec3 objectFront, glm::vec3 objectUp = glm::vec3(0.0f, 1.0f, 0.0f));
	void ProcessKeyPress(glm::vec3 planeMovement);
	void ProcessMouseMove(float xOffset, float yOffset);

	glm::vec3 getCameraPosition();
	void setCameraPosition(glm::vec3 cameraPosition);
	glm::vec3 getCameraFront();
	glm::vec3 getCameraUp();
	void setCameraFront(glm::vec3 cameraFront);

	void setCameraUp(glm::vec3 cameraUp);
	void setCameraRight(glm::vec3 cameraRight);

	void resetCamera();

	void setYaw(float y);
	void setPitch(float p);
	void resetRoll();


private:
	float yaw = -90.0f, pitch = 0.0f;
	float movementSpeed, mouseSensitivity, actualSpeed;
	glm::vec3 cameraPosition, cameraFront, cameraUp;
};

#endif