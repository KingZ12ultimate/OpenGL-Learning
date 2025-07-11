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


bool bump = true;
bool bumpKeyPressed = false;

void printVec(glm::vec3 v)
{
    std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

void processInput(GLFWwindow* window);

void renderScene(const Shader& shader);

glm::vec3 lightPos = glm::vec3(0.0f, 0.15f, 0.0f);
glm::vec3 lightColor = glm::vec3(1.0f);

bool debug = false;

int main(void)
{
    App::Initialize(1920, 1920);

	glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Shader bumpShader("resources/shaders/normal_mapping/normal_mapping.vert", "resources/shaders/normal_mapping/normal_mapping.frag");
	Shader skyboxShader("resources/shaders/skybox/skybox.vert", "resources/shaders/skybox/skybox.frag");

	unsigned int brickTexture = loadTexture("resources/textures/brickwall.jpg", true, true);
    unsigned int brickNormalTexture = loadTexture("resources/textures/brickwall_normal.jpg", true);
	unsigned int floorTexture = loadTexture("resources/textures/wood.png", true, true);
    
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
	
    bumpShader.use();
    bumpShader.setInt("diffuseTexture", 0);
    bumpShader.setInt("normal_texture", 1);
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

        bumpShader.use();
        bumpShader.setVec3("lightPos", lightPos);
        bumpShader.setVec3("viewPos", App::camera.position);
        bumpShader.setVec3("lightColor", lightColor);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brickTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, brickNormalTexture);
        renderScene(bumpShader);

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

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bumpKeyPressed)
    {
        bump = !bump;
        bumpKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        bumpKeyPressed = false;
    }

    App::camera.processKeyPresses(window, App::deltaTime);
}
#pragma endregion

#pragma region Rendering
void renderScene(const Shader& shader)
{
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.setBool("white", 0);
	shader.setMat4("model", model);
    renderPlane();
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.75f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.75f));
    shader.setMat4("model", model);
    renderCube();
    // light cube
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.05f));
    shader.setMat4("model", model);
    shader.setBool("white", 1);
    renderCube();
}
#pragma endregion
