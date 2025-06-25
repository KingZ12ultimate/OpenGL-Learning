#pragma once

#include "Shader.hpp"

extern unsigned int cubeVBO, planeVBO, quadVBO, skyboxVBO;
extern unsigned int cubeVAO, planeVAO, quadVAO, skyboxVAO;

/* Receives an array of vertex data in form:
    position - 3 floats,
    normal - 3 floats,
    texture coordinates - 2 floats,
   and constructs a list of "Vertex" structs.
   In addition, also calculates the tangents of the vertices.
   Note that the vertex data should contain 3 vertices for each triangle,
   including duplicates of the same vertices for different triangles.*/
void setupMesh(float data[], int dataSize, unsigned int& VAO, unsigned int& VBO);

void setupCube();
void renderCube();

void setupPlane();
void renderPlane();

void renderQuad();
void renderSkybox();

void renderScene(const Shader& shader);