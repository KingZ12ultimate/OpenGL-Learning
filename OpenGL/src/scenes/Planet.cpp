#include <iostream>

#include "utility/App.hpp"
#include "utility/Model.hpp"
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

void processInput(GLFWwindow* window);

int main(void)
{
    App::Initialize(1920, 1920);
    App::camera.setSpeed(100.0f);

	glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Shader planetShader("resources/shaders/base/base.vert", "resources/shaders/base/base.frag");
    Shader rockShader("resources/shaders/instancing/rock_instanced.vert", "resources/shaders/base/base.frag");

    Model rock("resources/models/rock/rock.obj");
	Model planet("resources/models/planet/planet.obj");

    #pragma region Setup Model Instanced Array
    glm::vec3 planetPos = glm::vec3(0.0f, -3.0f, 0.0f);
    unsigned int amount = 20000;
	glm::mat4* modelMatrices = new glm::mat4[amount];
	srand(static_cast<unsigned int>(glfwGetTime()));
	float radius = 500.0f;
	float offset = 100.0f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);

        // translation
		float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(200 * offset)) / 100.f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(200 * offset)) / 100.f - offset;
        float y = displacement * 0.4f;
        displacement = (rand() % (int)(200 * offset)) / 100.f - offset;
        float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z) - planetPos);

        // scaling
		float scale = (rand() % 20) / 100.f + 0.05f;
		model = glm::scale(model, glm::vec3(scale));
		
        // rotation
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, glm::radians(rotAngle), glm::vec3(0.4f, 0.8f, 0.2f));
		
        // now add to list of matrices
		modelMatrices[i] = model;
    }

    unsigned int modelMatricesBuffer;
	glGenBuffers(1, &modelMatricesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, modelMatricesBuffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    std::vector<Mesh> meshes = rock.getMeshes();
    for (unsigned int i = 0; i < meshes.size(); i++)
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

    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    planetShader.use();
    planetShader.setInt("texture1", 0);
    rockShader.use();
    rockShader.setInt("texture1", 0);

    while (!glfwWindowShouldClose(App::window))
    {
        App::UpdateTime();

        glfwPollEvents();
        processInput(App::window);

#pragma region draw scene
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LESS);

        glm::mat4 projection = App::camera.getProjection(App::SCREEN_WIDTH, App::SCREEN_HEIGHT);
        glm::mat4 view = App::camera.getView();
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

		// Draw the planet
		planetShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, planet.getTextures()[0].id);
        glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, planetPos);
		model = glm::scale(model, glm::vec3(4.0f));
		planetShader.setMat4("model", model);
		planet.Draw(planetShader);

		// Draw the rocks
		rockShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rock.getTextures()[0].id);
        rock.DrawInstanced(rockShader, amount);
#pragma endregion

        glfwSwapBuffers(App::window);
    }

    App::Clean();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    App::camera.processKeyPresses(window, deltaTime);
}
