#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	float fov = 45.0f;
	glm::vec3 cameraPos = glm::vec3(0.0f);
	glm::vec3 eulerAngles = glm::vec3(0.0f);
	glm::mat4 view, projection;

	Camera(glm::vec3 cameraPos, glm::vec3 eulerAngles, float fov);
	~Camera();

	glm::vec3 getCameraForward();
	glm::vec3 getCameraUp();
	glm::vec3 getCameraRight();

	glm::mat4 getView();
	glm::mat4 getProjection(unsigned int width, unsigned int height);

	void processMouseMovement(float xOffset, float yOffset);
	void processMouseScroll(float yOffset);

private:
	float minFov = 1.0f;
	float maxFov = 60.0f;
	float minPitch = -15.0f;
	float maxPitch = 75.0f;
	float sensitivity = 0.1f;
	glm::quat getQuat();
};

Camera::Camera(glm::vec3 cameraPos, glm::vec3 eulerAngles, float fov)
{
	this->cameraPos = cameraPos;

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
}

Camera::~Camera()
{
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
	return glm::lookAt(cameraPos, cameraPos + getCameraForward(), getCameraUp());
}

glm::mat4 Camera::getProjection(unsigned int width, unsigned int height)
{
	return glm::perspective(glm::radians(fov),
		(float)height / (float)width, 0.1f, 100.0f);;
}

glm::quat Camera::getQuat()
{
	float pitch = glm::radians(eulerAngles.x);
	float yaw = glm::radians(eulerAngles.y);
	float roll = glm::radians(eulerAngles.z);
	return glm::quat(glm::vec3(pitch, yaw, roll));
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
