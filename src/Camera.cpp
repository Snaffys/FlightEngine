#include <Camera.h>

Camera::Camera(glm::vec3 camera_position, glm::vec3 camera_front, glm::vec3 camera_up,
	float movement_speed, float mouse_sensitivity, float actual_speed)
	: cameraPosition(camera_position), cameraFront(camera_front), cameraUp(camera_up),
	movementSpeed(movement_speed), mouseSensitivity(mouse_sensitivity), actualSpeed(actual_speed) {}

void Camera::ProcessKeyPress(CameraMovement direction, float delta_time) {
	actualSpeed = delta_time * movementSpeed;
	// Moves camera
	if (direction == FORWARD)
		cameraPosition += actualSpeed * cameraFront;
	if (direction == BACKWARD)
		cameraPosition -= actualSpeed * cameraFront;
	if (direction == RIGHT)
		cameraPosition -= actualSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
	if (direction == LEFT)
		cameraPosition += actualSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
	if (direction == UP)
		cameraPosition += actualSpeed * cameraUp;
	if (direction == DOWN)
		cameraPosition -= actualSpeed * cameraUp;
}

void Camera::ProcessKeyPress(CameraMovement direction, float delta_time, glm::vec3 objectFront, glm::vec3 objectUp) {
	actualSpeed = delta_time * movementSpeed;
	// Moves camera
	if (direction == FORWARD)
		cameraPosition += actualSpeed * objectFront;
	if (direction == BACKWARD)
		cameraPosition -= actualSpeed * objectFront;
	if (direction == RIGHT)
		cameraPosition -= actualSpeed * glm::normalize(glm::cross(objectFront, objectUp));
	if (direction == LEFT)
		cameraPosition += actualSpeed * glm::normalize(glm::cross(objectFront, objectUp));
	if (direction == UP)
		cameraPosition += actualSpeed * objectUp;
	if (direction == DOWN)
		cameraPosition -= actualSpeed * objectUp;
}

void Camera::ProcessMouseMove(float x_offset, float y_offset) {
	x_offset *= mouseSensitivity;
	y_offset *= mouseSensitivity;
	// Changes pitch and yaw
	yaw += x_offset;
	pitch += y_offset;

	// Restrictons for the pitch
	if (pitch > 80.0f)
		pitch = 80.0f;
	if (pitch < -80.0f)
		pitch = -80.0f;

	// Calculates directions
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	// Sets camera_front
	cameraFront = glm::normalize(direction);
}

glm::vec3 Camera::getCameraPosition() {
	return cameraPosition;
}

void Camera::setCameraPosition(glm::vec3 cameraPosition) {
	this->cameraPosition = cameraPosition;
}

glm::vec3 Camera::getCameraFront() {
	return cameraFront;
}

void Camera::setCameraFront(glm::vec3 cameraFront) {
	this->cameraFront = cameraFront;
}
glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
}

void Camera::resetCamera() {
	yaw = -90.0f;
	pitch = 0.0f;
};


