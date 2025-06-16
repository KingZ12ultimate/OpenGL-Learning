#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h> // include glad to get the required OpenGL headers
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	// the program ID
	unsigned int ID;
	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
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

	// deconstructor delete the shader program and cleans resources
	~Shader()
	{
		glDeleteProgram(ID);
	}

	// use/activate the shader
	void use()
	{
		glUseProgram(ID);
	}

	// utility uniform functions
	
	// set uniform bool
	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	// set uniform int
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	// set uniform float
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	// set uniform vec2
	void setVec2(const std::string& name, float v0, float v1) const
	{
		const float* val = glm::value_ptr(glm::vec2(v0, v1));
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, val);
	}

	// set uniform vec2
	void setVec2(const std::string& name, glm::vec2 v) const
	{
		const float* val = glm::value_ptr(v);
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, val);
	}

	// set uniform vec3
	void setVec3(const std::string& name, float v0, float v1, float v2) const
	{
		const float* val = glm::value_ptr(glm::vec3(v0, v1, v2));
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, val);
	}

	// set uniform vec3
	void setVec3(const std::string& name, glm::vec3 v) const
	{
		const float* val = glm::value_ptr(v);
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, val);
	}

	// set uniform vec4
	void setVec4(const std::string& name, float v0, float v1, float v2, float v3) const
	{
		const float* val = glm::value_ptr(glm::vec4(v0, v1, v2, v3));
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, val);
	}

	// set uniform vec4
	void setVec4(const std::string& name, glm::vec4 v) const
	{
		const float* val = glm::value_ptr(v);
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, val);
	}

	// set uniform mat2
	void setMat2(const std::string& name, glm::mat2 matrix) const
	{
		const float* val = glm::value_ptr(matrix);
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()),
			1, GL_FALSE, val);
	}

	// set uniform mat3
	void setMat3(const std::string& name, glm::mat3 matrix) const
	{
		const float* val = glm::value_ptr(matrix);
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()),
			1, GL_FALSE, val);
	}

	// set uniform mat4
	void setMat4(const std::string& name, glm::mat4 matrix) const
	{
		const float* val = glm::value_ptr(matrix);
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()),
			1, GL_FALSE, val);
	}
};
#endif
