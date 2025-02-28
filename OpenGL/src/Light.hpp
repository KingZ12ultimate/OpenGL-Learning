#pragma once
#include <glm/glm.hpp>

// Base class that represents a light in the scene
class Light
{
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
	{
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
	}
};


class DirectionalLight : public Light
{
public:
	glm::vec3 direction;

	DirectionalLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction)
		: Light(ambient, diffuse, specular)
	{
		this->direction = direction;
	}
};


class PointLight : public Light
{
public:
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;

	PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position,
		float constant, float linear, float quadratic) : Light(ambient, diffuse, specular)
	{
		this->position = position;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
	}
};


class SpotLight : public PointLight
{
public:
	float cutOff;
	float outerCutOff;

	SpotLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position,
		float constant, float linear, float quadratic, float cutOff, float outerCutOff) : 
		PointLight(ambient, diffuse, specular, position, constant, linear, quadratic)
	{
		this->cutOff = cutOff;
		this->outerCutOff = outerCutOff;
	}
};

