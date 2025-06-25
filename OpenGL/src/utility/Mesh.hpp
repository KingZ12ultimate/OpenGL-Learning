#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Shader.hpp"

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	// mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	unsigned int VAO;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(const Shader& shader);
	void DrawInstanced(const Shader& shader, int amount);
	void Clean();

private:
	// render data
	unsigned int VBO, EBO;

	void SetupMesh();
	void SetupTexturesForDraw(const Shader& shader);
};
