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

void processInput(GLFWwindow* window);

int main(void)
{
    App::Initialize(1920, 1920);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Shader shader("resources/shaders/base/base.vert", "resources/shaders/base/base.frag");
	Shader containerShader("resources/shaders/base/container.vert", "resources/shaders/base/container.frag");
	Shader grassShader("resources/shaders/base/base.vert", "resources/shaders/blend/blend.frag");
	Shader skyboxShader("resources/shaders/skybox/skybox.vert", "resources/shaders/skybox/skybox.frag");

    Model m_model("resources/models/backpack/backpack.obj");

    std::vector<glm::vec3> vegetation;
    vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
    vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
    vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

    // load textures
    // -------------
    unsigned int cubeTexture = loadTexture("resources/textures/container.jpg", true);
    unsigned int floorTexture = loadTexture("resources/textures/metal.png", true);
	unsigned int grassTexture = loadTexture("resources/textures/grass.png", false);

    std::string skyboxPath = "resources/textures/skyboxes/skybox_bright/";
    std::vector<std::string> faces = {
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"
    };
    for (unsigned int i = 0; i < faces.size(); i++)
        faces[i] = skyboxPath + faces[i];

    unsigned int skyboxTexture = loadCubeMap(faces);

	unsigned int uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    // shader configuration
    // --------------------
    grassShader.use();
    grassShader.setInt("texture1", 0);

    while (!glfwWindowShouldClose(App::window))
    {
        App::UpdateTime();
        
        glfwPollEvents();
        processInput(App::window);

        #pragma region draw scene
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LESS);

        glm::mat4 projection = App::camera.getProjection(1920, 1920);
        glm::mat4 view = App::camera.getView();
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);

        // floor
        shader.use();
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        shader.setMat4("model", glm::mat4(1.0f));
        renderPlane();
        glBindVertexArray(0);

        // cubes
		containerShader.use();
        containerShader.setVec3("cameraPos", App::camera.position);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 1.001f, -1.0f));
        containerShader.setMat4("model", model);
        renderCube();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(6.0f, 1.001f, 0.0f));
        containerShader.setMat4("model", model);
        renderCube();

        //backpack
        model = glm::translate(model, glm::vec3(-6.0f, 3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        containerShader.setMat4("model", model);
        m_model.Draw(shader);

		// grass
		glDisable(GL_CULL_FACE);
		grassShader.use();
		glBindTexture(GL_TEXTURE_2D, grassTexture);

        for (unsigned int i = 0; i < vegetation.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, vegetation[i]);
            grassShader.setMat4("model", model);
            renderGrass();
        }
        glEnable(GL_CULL_FACE);

        // skybox
		view = glm::mat4(glm::mat3(App::camera.getView()));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		skyboxShader.use();
        glDepthFunc(GL_LEQUAL);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        renderSkybox();
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

    App::camera.processKeyPresses(window, App::deltaTime);
}
