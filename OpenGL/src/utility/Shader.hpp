#ifndef SHADER_H
#define SHADER_H
#include <string>

class Shader
{
public:
	// the program ID
	unsigned int ID;
	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	
	// deconstructor delete the shader program and cleans resources
	~Shader();

	// use/activate the shader
	void use() const;

	// utility uniform functions
	
	// set uniform bool
	void setBool(const std::string& name, bool value) const;

	// set uniform int
	void setInt(const std::string& name, int value) const;

	// set uniform float
	void setFloat(const std::string& name, float value) const;

	// set uniform vec2
	void setVec2(const std::string& name, float v0, float v1) const;

	// set uniform vec2
	void setVec2(const std::string& name, glm::vec2 v) const;

	// set uniform vec3
	void setVec3(const std::string& name, float v0, float v1, float v2) const;

	// set uniform vec3
	void setVec3(const std::string& name, glm::vec3 v) const;

	// set uniform vec4
	void setVec4(const std::string& name, float v0, float v1, float v2, float v3) const;

	// set uniform vec4
	void setVec4(const std::string& name, glm::vec4 v) const;

	// set uniform mat2
	void setMat2(const std::string& name, glm::mat2 matrix) const;

	// set uniform mat3
	void setMat3(const std::string& name, glm::mat3 matrix) const;

	// set uniform mat4
	void setMat4(const std::string& name, glm::mat4 matrix) const;
};
#endif
