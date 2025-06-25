#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// includes for Dear ImGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.hpp"


namespace App
{
    extern GLFWwindow* window;
    extern bool firstMouse;
    extern bool initialized;
    extern float lastX, lastY;
    extern unsigned int SCREEN_WIDTH, SCREEN_HEIGHT;
    extern float near, far;
    extern Camera camera;
    extern ImGuiIO* io;

    void Initialize(unsigned int screenWidth = 1080, unsigned int screenHeight = 1080, const char* title = "LearnOpenGL");
    void Clean();

    void glfw_error_callback(int error, const char* description);
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void mouse_callback(GLFWwindow* window, double xPos, double yPos);
    void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
}
