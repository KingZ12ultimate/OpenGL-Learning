#include <iostream>

#include "utility/App.hpp"
#include "utility/ComputeShader.hpp"
#include "utility/Shader.hpp"
#include "utility/Render.hpp"


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

int main(void)
{
    App::Initialize(1024, 1024);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    /*int data[6];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &data[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &data[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &data[2]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &data[3]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &data[4]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &data[5]);

    std::cout << "Group count: " << data[0] << ", " << data[1] << ", " << data[2] << std::endl;
    std::cout << "Group size: " << data[3] << ", " << data[4] << ", " << data[5] << std::endl;*/

    ComputeShader computeShader("resources/shaders/compute/beginner.comp");
    Shader quadShader("resources/shaders/base/quad.vert", "resources/shaders/base/quad.frag");

    const unsigned int TEXTURE_SIZE = 1024;
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    quadShader.use();
    quadShader.setInt("screenTexture", 0);

    while (!glfwWindowShouldClose(App::window))
    {
        App::UpdateTime();
        App::UpdateMouseStatus();

        processInput(App::window);

        if (glfwGetWindowAttrib(App::window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        glViewport(0, 0, App::SCREEN_WIDTH, App::SCREEN_HEIGHT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static float speed = 1.0f;
        computeShader.Use();
        computeShader.SetFloat("t", App::currentFrame);
        computeShader.SetFloat("speed", speed);
        glDispatchCompute(TEXTURE_SIZE / 32, TEXTURE_SIZE / 32, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        quadShader.use();
        renderQuad();

        if (App::cursor)
        {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Hello, world!");
            ImGui::Text("Press \"P\" to toggle cursor.");
            ImGui::SliderFloat("Speed", &speed, 0.0f, 4.0f);
            ImGui::Text("Current FPS is: %.1f (as measured by ImGui).", App::io->Framerate);
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Update and Render additional Platform Windows
            if (App::io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }
        }

        glfwSwapBuffers(App::window);
        glfwPollEvents();
    }

    App::Clean();
    return 0;
}

#pragma region Input Handling
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // App::camera.processKeyPresses(window, deltaTime);
}
#pragma endregion
