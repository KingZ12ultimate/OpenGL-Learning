#include <iostream>
#include <vector>

#include "utility/App.hpp"
#include "utility/Model.hpp"
#include "utility/Render.hpp"
#include "utility/Shader.hpp"
#include "utility/Texture.hpp"


#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllexport) unsigned int NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif


void printVec(glm::vec3& v)
{
    std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

void processInput(GLFWwindow* window);

glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);

bool debug = false;

int main(void)
{
    App::Initialize(1920, 1920);

    // glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Shader gBufferShader("resources/shaders/deferred/geometry_pass.vert", "resources/shaders/deferred/geometry_pass.frag");
    Shader lightingPassShader("resources/shaders/base/quad.vert", "resources/shaders/deferred/lighting_pass.frag");
    Shader lightCubeShader("resources/shaders/instancing/light_cube_instanced.vert", "resources/shaders/instancing/light_cube_instanced.frag");
    Shader skyboxShader("resources/shaders/skybox/skybox.vert", "resources/shaders/skybox/skybox.frag");

    Model backpack("resources/models/backpack/backpack.obj");

    #pragma region Instancing data
    int amount = 10;
    float offset = 5.0f;
    glm::mat4* modelMatrices = new glm::mat4[amount * amount];
    for (int i = 0; i < amount; i++)
    {
        float z = (i + 0.5f - 0.5f * amount) * offset;
        for (int j = 0; j < amount; j++)
        {
            float x = (j + 0.5f - 0.5f * amount) * offset;
            glm::vec3 pos = glm::vec3(x, 0.0f, z);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            modelMatrices[i * amount + j] = model;
        }
    }

    unsigned int modelMatricesBuffer;
    glGenBuffers(1, &modelMatricesBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, modelMatricesBuffer);
    glBufferData(GL_ARRAY_BUFFER, amount * amount * sizeof(glm::mat4), modelMatrices, GL_STATIC_DRAW);

    std::vector<Mesh> meshes = backpack.getMeshes();
    for (int i = 0; i < meshes.size(); i++)
    {
        unsigned int VAO = meshes[i].VAO;
        glBindVertexArray(VAO);
        std::size_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)vec4Size);
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
    #pragma endregion

    #pragma region Lights
    struct Light {
        glm::mat4 world;
        glm::vec3 color;
    };

    int numLights = 8;
    offset = 6.0f;
    float y = 3.5f;
    lightingPassShader.use();
    srand(static_cast<unsigned int>(glfwGetTime()));
    Light* lights = new Light[numLights * numLights];
    for (int i = 0; i < numLights; i++)
    {
        float z = (i + 0.5f - 0.5f * numLights) * offset;
        for (int j = 0; j < numLights; j++)
        {
            float x = (j + 0.5f - 0.5f * numLights) * offset;
            glm::vec3 pos = glm::vec3(x, y, z);
            
            float r = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // random between 0.5 and 1.0
            float g = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // random between 0.5 and 1.0
            float b = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // random between 0.5 and 1.0
            glm::vec3 color = glm::vec3(r, g, b);
            lights[i * numLights + j].color = color;

            lightingPassShader.setVec3("lights[" + std::to_string(i * numLights + j) + "].position", pos);
            lightingPassShader.setVec3("lights[" + std::to_string(i * numLights + j) + "].color", color);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::scale(model, glm::vec3(0.1f));
            lights[i * numLights + j].world = model;
        }
    }
    lightingPassShader.setInt("gPosition", 0);
    lightingPassShader.setInt("gNormal", 1);
    lightingPassShader.setInt("gAlbedoSpec", 2);

    unsigned int lightDataBuffer;
    glGenBuffers(1, &lightDataBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, lightDataBuffer);
    glBufferData(GL_ARRAY_BUFFER, numLights * numLights * sizeof(Light), lights, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Light), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Light), (void*)vec4Size);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Light), (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Light), (void*)(3 * vec4Size));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(Light), (void*)(4 * vec4Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);

    glBindVertexArray(0);
    delete[] lights;
    lights = nullptr;
    #pragma endregion

    #pragma region Skybox
    std::string skyboxPath = "resources/textures/skyboxes/skybox_space/";
    std::vector<std::string> faces = {
        "right.png",
        "left.png",
        "top.png",
        "bottom.png",
        "front.png",
        "back.png"
    };
    for (unsigned int i = 0; i < faces.size(); i++)
        faces[i] = skyboxPath + faces[i];

    unsigned int skyboxTexture = loadCubeMap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    #pragma endregion

    #pragma region Geometry Buffer
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gColorSpec;
    
    // position buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, App::SCREEN_WIDTH, App::SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    
    // normal buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, App::SCREEN_WIDTH, App::SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    
    // albedo-spec buffer
    glGenTextures(1, &gColorSpec);
    glBindTexture(GL_TEXTURE_2D, gColorSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, App::SCREEN_WIDTH, App::SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // depth buffer
    unsigned int depthRBO;
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App::SCREEN_WIDTH, App::SCREEN_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    #pragma endregion

    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    glViewport(0, 0, App::SCREEN_WIDTH, App::SCREEN_HEIGHT);
    while (!glfwWindowShouldClose(App::window))
    {
        App::UpdateTime();
        
        glfwPollEvents();
        processInput(App::window);

#pragma region draw scene
        // first pass (geometry pass)
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, App::SCREEN_WIDTH, App::SCREEN_HEIGHT);

        glm::mat4 projection = App::camera.getProjection(App::SCREEN_WIDTH, App::SCREEN_HEIGHT);
        glm::mat4 view = App::camera.getView();
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        
        gBufferShader.use();
        backpack.DrawInstanced(gBufferShader, amount * amount);

        // second pass (lighting pass)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gColorSpec);
        lightingPassShader.use();
        lightingPassShader.setVec3("viewPos", App::camera.position);
        renderQuad();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, App::SCREEN_WIDTH, App::SCREEN_HEIGHT, 0, 0, App::SCREEN_WIDTH, App::SCREEN_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        lightCubeShader.use();
        lightCubeShader.setBool("white", true);
        lightCubeShader.setVec3("color", glm::vec3(1.0f));
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, numLights* numLights);
        glBindVertexArray(0);
#pragma endregion

        glfwSwapBuffers(App::window);
    }

    App::Clean();
    return 0;
}

#pragma region Input
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    App::camera.processKeyPresses(window, App::deltaTime);
}
#pragma endregion
