#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <string>

class ComputeShader
{
public:
	unsigned int ID;

	ComputeShader(const char* path);
	~ComputeShader();

	void Use() const;

	// set uniform bool
	void SetBool(const std::string& name, bool value);

	// set uniform int
	void SetInt(const std::string& name, int value);

	// set uniform float
	void SetFloat(const std::string& name, float value);

	// set uniform vec2
	void SetVec2(const std::string& name, float v0, float v1);

	// set uniform vec2
	void SetVec2(const std::string& name, glm::vec2 v);

	// set uniform vec3
	void SetVec3(const std::string& name, float v0, float v1, float v2);

	// set uniform vec3
	void SetVec3(const std::string& name, glm::vec3 v);

	// set uniform vec4
	void SetVec4(const std::string& name, float v0, float v1, float v2, float v3);

	// set uniform vec4
	void SetVec4(const std::string& name, glm::vec4 v);

	// set uniform mat2
	void SetMat2(const std::string& name, glm::mat2 matrix);

	// set uniform mat3
	void SetMat3(const std::string& name, glm::mat3 matrix);

	// set uniform mat4
	void SetMat4(const std::string& name, glm::mat4 matrix);

private:
	std::unordered_map<std::string, unsigned int> locationTable;

	// checks whether the uniform "name" already has its location in locationTable.
	// If it doesn't, it inserts its location bound the key "name".
	void CheckUniformWithTable(std::string name);
};

