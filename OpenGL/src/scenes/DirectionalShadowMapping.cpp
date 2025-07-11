#include <iostream>

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


bool blinn = false;
bool blinnKeyPressed = false;

void processInput(GLFWwindow* window);

void renderScene(const Shader& shader);

glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

bool debug = false;

int main(void)
{
    App::Initialize(1920, 1920);

	glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

	Shader simpleDepthShader("resources/shaders/directional_shadows/simple_depth.vert", "resources/shaders/directional_shadows/simple_depth.frag");
	Shader shadowShader("resources/shaders/directional_shadows/depth_shadows.vert", "resources/shaders/directional_shadows/depth_shadows.frag");
    Shader quadShader("resources/shaders/base/quad.vert", "resources/shaders/base/quad.frag");

	unsigned int woodTexture = loadTexture("resources/textures/wood.png", true, true);

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
	shadowShader.use();
	shadowShader.setInt("diffuseTexture", 0);
    shadowShader.setInt("shadowMap", 1);
	quadShader.use();
    quadShader.setInt("screenTexture", 0);

    while (!glfwWindowShouldClose(App::window))
    {
        App::UpdateTime();

        glfwPollEvents();
        processInput(App::window);

#pragma region draw scene
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // first pass
		float near = 1.0f, far = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near, far);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		renderScene(simpleDepthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, App::SCREEN_WIDTH, App::SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// second pass
        glm::mat4 projection = App::camera.getProjection(App::SCREEN_WIDTH, App::SCREEN_HEIGHT);
        glm::mat4 view = App::camera.getView();
        shadowShader.use();
		shadowShader.setMat4("projection", projection);
		shadowShader.setMat4("view", view);
		shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		shadowShader.setVec3("lightPos", lightPos);
        shadowShader.setVec3("viewPos", App::camera.position);
		shadowShader.setVec4("fogColor", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
		shadowShader.setFloat("nearPlane", near);
		shadowShader.setFloat("farPlane", far);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
		renderScene(shadowShader);
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

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn = !blinn;
        blinnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        blinnKeyPressed = false;
    }

    App::camera.processKeyPresses(window, App::deltaTime);
}
#pragma endregion

#pragma region Rendering
void renderScene(const Shader& shader)
{
    glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);
    renderPlane();

    model = glm::mat4(1.0f);
    float x = 2.0f * sinf(static_cast<float>(glfwGetTime()));
    model = glm::translate(model, glm::vec3(x, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    shader.setMat4("model", model);
    renderCube();
}
#pragma endregion
