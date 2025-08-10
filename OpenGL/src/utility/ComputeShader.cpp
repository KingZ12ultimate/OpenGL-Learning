#include <glad/glad.h> // include glad to get the required OpenGL headers
#include <fstream>
#include <sstream>
#include <iostream>

#include "ComputeShader.hpp"

ComputeShader::ComputeShader(const char* path)
{
	std::string shaderCode;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		shaderFile.open(path);
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* code = shaderCode.c_str();
	
	unsigned int compute;
	int success;
	char infoLog[512];

	compute = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute, 1, &code, NULL);
	glCompileShader(compute);
	glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(compute, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED::\n" <<
			infoLog << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, compute);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED::\n" <<
			infoLog << std::endl;
	}

	glDeleteShader(compute);
}

ComputeShader::~ComputeShader()
{
	glDeleteProgram(ID);
}

void ComputeShader::Use() const
{
	glUseProgram(ID);
}

void ComputeShader::CheckUniformWithTable(std::string name)
{
	if (locationTable.contains(name))
		return;
	locationTable[name] = glGetUniformLocation(ID, name.c_str());
}

void ComputeShader::SetBool(const std::string& name, bool value)
{
	CheckUniformWithTable(name);
	glUniform1i(locationTable[name], value);
}

void ComputeShader::SetInt(const std::string& name, int value)
{
	CheckUniformWithTable(name);
	glUniform1i(locationTable[name], value);
}

void ComputeShader::SetFloat(const std::string& name, float value)
{
	CheckUniformWithTable(name);
	glUniform1f(locationTable[name], value);
}

void ComputeShader::SetVec2(const std::string& name, float v0, float v1)
{
	CheckUniformWithTable(name);
	glUniform2f(locationTable[name], v0, v1);
}

void ComputeShader::SetVec2(const std::string& name, glm::vec2 v)
{
	CheckUniformWithTable(name);
	glUniform2fv(locationTable[name], 1, glm::value_ptr(v));
}

void ComputeShader::SetVec3(const std::string& name, float v0, float v1, float v2)
{
	CheckUniformWithTable(name);
	glUniform3f(locationTable[name], v0, v1, v2);
}

void ComputeShader::SetVec3(const std::string& name, glm::vec3 v)
{
	CheckUniformWithTable(name);
	glUniform3fv(locationTable[name], 1, glm::value_ptr(v));
}

void ComputeShader::SetVec4(const std::string& name, float v0, float v1, float v2, float v3)
{
	CheckUniformWithTable(name);
	glUniform4f(locationTable[name], v0, v1, v2, v3);
}

void ComputeShader::SetVec4(const std::string& name, glm::vec4 v)
{
	CheckUniformWithTable(name);
	glUniform4fv(locationTable[name], 1, glm::value_ptr(v));
}

void ComputeShader::SetMat2(const std::string& name, glm::mat2 matrix)
{
	CheckUniformWithTable(name);
	glUniformMatrix2fv(locationTable[name], 1, GL_FALSE, glm::value_ptr(matrix));
}

void ComputeShader::SetMat3(const std::string& name, glm::mat3 matrix)
{
	CheckUniformWithTable(name);
	glUniformMatrix3fv(locationTable[name], 1, GL_FALSE, glm::value_ptr(matrix));
}

void ComputeShader::SetMat4(const std::string& name, glm::mat4 matrix)
{
	CheckUniformWithTable(name);
	glUniformMatrix4fv(locationTable[name], 1, GL_FALSE, glm::value_ptr(matrix));
}
