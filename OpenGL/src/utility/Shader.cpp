#include <glad/glad.h> // include glad to get the required OpenGL headers
#include <fstream>
#include <sstream>
#include <iostream>

#include "Shader.hpp"

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	//1. retrieve the shader source code from filePath
	std::string vertexCode;
	std::string geometryCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream gShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		//open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		//read file’s buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//close file handlers
		vShaderFile.close();
		fShaderFile.close();
		//convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	const char* gShaderCode = nullptr;
	if (geometryPath != nullptr)
	{
		try
		{
			// open file
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			//read file’s buffer contents into streams
			gShaderStream << gShaderFile.rdbuf();
			//close file handler
			gShaderFile.close();
			//convert stream into string
			geometryCode = gShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		gShaderCode = geometryCode.c_str();
	}

	// 2. compile shaders
	unsigned int vertex, fragment, geometry = 0;
	int success;
	char infoLog[512];

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED::\n" <<
			infoLog << std::endl;
	}

	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED::\n" <<
			infoLog << std::endl;
	}

	// geometry shader (if exists)
	if (gShaderCode != nullptr)
	{
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		// print compile errors if any
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED::\n" <<
				infoLog << std::endl;
		}
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometry != 0)
		glAttachShader(ID, geometry);
	glLinkProgram(ID);
	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED::\n" <<
			infoLog << std::endl;
	}

	// delete shaders. they're linked into our program and no longer needed.
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::use() const
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, float v0, float v1) const
{
	const float* val = glm::value_ptr(glm::vec2(v0, v1));
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, val);
}

void Shader::setVec2(const std::string& name, glm::vec2 v) const
{
	const float* val = glm::value_ptr(v);
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, val);
}

void Shader::setVec3(const std::string& name, float v0, float v1, float v2) const
{
	const float* val = glm::value_ptr(glm::vec3(v0, v1, v2));
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, val);
}

void Shader::setVec3(const std::string& name, glm::vec3 v) const
{
	const float* val = glm::value_ptr(v);
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, val);
}

void Shader::setVec4(const std::string& name, float v0, float v1, float v2, float v3) const
{
	const float* val = glm::value_ptr(glm::vec4(v0, v1, v2, v3));
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, val);
}

void Shader::setVec4(const std::string& name, glm::vec4 v) const
{
	const float* val = glm::value_ptr(v);
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, val);
}

void Shader::setMat2(const std::string& name, glm::mat2 matrix) const
{
	const float* val = glm::value_ptr(matrix);
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()),
		1, GL_FALSE, val);
}

void Shader::setMat3(const std::string& name, glm::mat3 matrix) const
{
	const float* val = glm::value_ptr(matrix);
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()),
		1, GL_FALSE, val);
}

void Shader::setMat4(const std::string& name, glm::mat4 matrix) const
{
	const float* val = glm::value_ptr(matrix);
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()),
		1, GL_FALSE, val);
}
