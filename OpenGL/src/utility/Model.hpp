#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.hpp"


class Model
{
public:
	Model(const char* path);
	~Model();

	void Draw(const Shader& shader);
	void DrawInstanced(const Shader& shader, int amount);
	
	std::vector<Mesh>& getMeshes();
	std::vector<Texture>& getTextures();
private:
	// model data
	std::vector<Mesh> meshes;
	std::vector<Texture> textures_loaded;
	std::string directory;

	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
