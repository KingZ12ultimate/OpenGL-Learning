#include <iostream>
#include <vector>

#include "utility/App.hpp"
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


void printVec(glm::vec3 v)
{
    std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

void processInput(GLFWwindow* window);

void renderScene(const Shader& shader);

glm::vec3 lightPos = glm::vec3(0.0f);

bool debug = false;

int main(void)
{
    App::Initialize(1920, 1920);

	glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Shader baseShader("resources/shaders/base/base.vert", "resources/shaders/base/base.frag");
    Shader parallaxShader("resources/shaders/parallax_mapping/parallax_mapping.vert", "resources/shaders/parallax_mapping/parallax_mapping.frag");
	Shader skyboxShader("resources/shaders/skybox/skybox.vert", "resources/shaders/skybox/skybox.frag");

	unsigned int brickTexture = loadTexture("resources/textures/wood.png", true, true);
    unsigned int brickNormalTexture = loadTexture("resources/textures/toy_box_normal.png", true);
    unsigned int brickDispTexture = loadTexture("resources/textures/toy_box_disp.png", true);
    
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

    unsigned int uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
	
    baseShader.use();
    baseShader.setBool("white", 1);
    parallaxShader.use();
    parallaxShader.setInt("diffuseTexture", 0);
    parallaxShader.setInt("normalMap", 1);
    parallaxShader.setInt("depthMap", 2);
    skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

    while (!glfwWindowShouldClose(App::window))
    {
        App::UpdateTime();

        glfwPollEvents();
        processInput(App::window);

#pragma region draw scene
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LESS);

		glm::mat4 projection = App::camera.getProjection(App::SCREEN_WIDTH, App::SCREEN_HEIGHT);
		glm::mat4 view = App::camera.getView();
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

        float radius = 3.0f;
        float speed = 1.0f;
        lightPos.x = radius * cosf(speed * App::currentFrame);
        lightPos.z = radius * sinf(speed * App::currentFrame);

        parallaxShader.use();
        parallaxShader.setVec3("lightPos", lightPos);
        parallaxShader.setVec3("viewPos", App::camera.position);
        parallaxShader.setFloat("heightScale", 0.25f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brickTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, brickNormalTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, brickDispTexture);
        renderScene(parallaxShader);

        // light cube
        baseShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.05f));
        baseShader.setMat4("model", model);
        renderCube();

        // skybox
        skyboxShader.use();
		view = glm::mat4(glm::mat3(view)); // remove translation from view matrix
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glDepthFunc(GL_LEQUAL);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        renderSkybox();
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

#pragma region Rendering
void renderScene(const Shader& shader)
{
    // cube
    glm::mat4 model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.75f));
    shader.setMat4("model", model);
    renderCube();
}
#pragma endregion
