#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera
{
public:
	float fov = 45.0f;
	float near = 0.1f, far = 1000.0f;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 eulerAngles = glm::vec3(0.0f);
	glm::mat4 view, projection;

	Camera(glm::vec3 cameraPos, glm::vec3 eulerAngles, float fov, float near, float far);

	glm::vec3 getCameraForward();
	glm::vec3 getCameraUp();
	glm::vec3 getCameraRight();
	glm::quat getQuat();

	glm::mat4 getView();
	glm::mat4 getProjection(unsigned int width, unsigned int height);

	void processKeyPresses(GLFWwindow* window, float deltaTime);
	void processMouseMovement(float xOffset, float yOffset);
	void processMouseScroll(float yOffset);
	void setSpeed(float speed);

private:
	float minFov = 1.0f;
	float maxFov = 60.0f;
	float minPitch = -89.0f;
	float maxPitch = 89.0f;
	float sensitivity = 0.1f;
	float speed = 10.0f;
};
