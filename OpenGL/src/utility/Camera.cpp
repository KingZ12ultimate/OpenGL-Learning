#include <glm/gtc/matrix_transform.hpp>
#include "Camera.hpp"

Camera::Camera(glm::vec3 cameraPos, glm::vec3 eulerAngles, float fov, float near, float far)
{
	this->position = cameraPos;

	if (eulerAngles.x > this->maxPitch)
		eulerAngles.x = this->maxPitch;
	else if (eulerAngles.x < this->minPitch)
		eulerAngles.x = this->minPitch;

	this->eulerAngles = eulerAngles;

	if (fov > this->maxFov)
		fov = this->maxFov;
	else if (fov < this->minFov)
		fov = this->minFov;
	this->fov = fov;

	this->near = near;
	this->far = far;
}

glm::vec3 Camera::getCameraForward()
{
	return getQuat() * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 Camera::getCameraUp()
{
	return getQuat() * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Camera::getCameraRight()
{
	return getQuat() * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::mat4 Camera::getView()
{
	return glm::lookAt(position, position + getCameraForward(), getCameraUp());
}

glm::mat4 Camera::getProjection(unsigned int width, unsigned int height)
{
	return glm::perspective(glm::radians(fov),
		(float)height / (float)width, near, far);
}

glm::quat Camera::getQuat()
{
	float pitch = glm::radians(eulerAngles.x);
	float yaw = glm::radians(eulerAngles.y);
	float roll = glm::radians(eulerAngles.z);
	return glm::quat(glm::vec3(pitch, yaw, roll));
}

void Camera::processKeyPresses(GLFWwindow* window, float deltaTime)
{
	float delta = speed * deltaTime;
	glm::vec3 right = getCameraRight();
	glm::vec3 forward = getCameraForward();
	glm::vec3 up = glm::cross(right, forward);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += delta * forward;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= delta * forward;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= delta * right;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += delta * right;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		position += delta * up;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		position -= delta * up;
}

void Camera::processMouseMovement(float xOffset, float yOffset)
{
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	eulerAngles.y -= xOffset;
	eulerAngles.x -= yOffset;

	if (eulerAngles.x > maxPitch)
		eulerAngles.x = maxPitch;
	else if (eulerAngles.x < minPitch)
		eulerAngles.x = minPitch;
}

void Camera::processMouseScroll(float yOffset)
{
	fov -= yOffset;
	if (fov > maxFov)
		fov = maxFov;
	else if (fov < minFov)
		fov = minFov;
}

void Camera::setSpeed(float speed)
{
	speed = fabsf(speed);
}