#pragma once

#include <iostream>
#include <glad/glad.h>
#include <vector>
#include "stb_image.h"

namespace TextureType {
    constexpr const char* Diffuse  = "texture_diffuse";
    constexpr const char* Specular = "texture_specular";
    constexpr const char* Normal   = "texture_normal";
    // Add more as needed
}

unsigned int loadTexture(char const* path, bool repeat, bool gamma = false);
unsigned int loadCubeMap(std::vector<std::string> faces);
unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
