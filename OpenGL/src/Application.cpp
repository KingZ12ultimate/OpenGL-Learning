//#include <iostream>
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include "Model.hpp"
//#include "Shader.hpp"
//#include "Camera.hpp"
//
//
//const char* vertexShaderPath = "resources/shaders/vertex.glsl";
//const char* fragmentShaderPath = "resources/shaders/fragment.glsl";
//
//const unsigned int WIDTH = 1920;
//const unsigned int HEIGHT = 1920;
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xPos, double yPos);
//void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
//void processInput(GLFWwindow* window);
//
//float deltaTime = 0.0f, lastFrame = 0.0f;
//
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f), 45.0f);
//float lastX = (float)WIDTH / 2.0f, lastY = (float)HEIGHT / 2.0f;
//bool firstMouse = true;
//
//int main(void)
//{
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
//    if (window == NULL)
//    {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//
//    // Sets the viewport size on the window that OpenGL will render to.
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//    glfwSetCursorPosCallback(window, mouse_callback);
//    glfwSetScrollCallback(window, scroll_callback);
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
//
//    // Check for OpenGL errors
//    GLenum err;
//    while ((err = glGetError()) != GL_NO_ERROR)
//    {
//        std::cout << "OpenGL error: " << err << std::endl;
//    }
//
//    stbi_set_flip_vertically_on_load(true);
//
//    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
//    glEnable(GL_DEPTH_TEST);
//
//    Shader shader(vertexShaderPath, fragmentShaderPath);
//    Model m_model("resources/models/backpack/backpack.obj");
//
//    while (!glfwWindowShouldClose(window))
//    {
//        float currentFrame = static_cast<float>(glfwGetTime());
//        deltaTime = currentFrame - lastFrame;
//        lastFrame = currentFrame;
//
//        processInput(window);
//
//        // Rendering
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        shader.use();
//        shader.setVec3("viewPos", camera.position);
//        shader.setFloat("material.shininess", 32.0f);
//
//        // directional light
//        shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
//        shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
//        shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
//        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
//
//        glm::mat4 projection = camera.getProjection(WIDTH, HEIGHT);
//        glm::mat4 view = camera.getView();
//        shader.setMat4("projection", projection);
//        shader.setMat4("view", view);
//        
//        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f));
//        model = glm::scale(model, glm::vec3(1.0f));
//        shader.setMat4("model", model);
//        m_model.Draw(shader);
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    glfwTerminate();
//
//    return 0;
//}
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    // Sets the viewport size on the window that OpenGL will render to.
//    glViewport(0, 0, width, height);
//}
//
//void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
//{
//    float xPos = static_cast<float>(xPosIn);
//    float yPos = static_cast<float>(yPosIn);
//    
//    if (firstMouse)
//    {
//        lastX = xPos;
//        lastY = yPos;
//        firstMouse = false;
//    }
//    
//    float xOffset = xPos - lastX;
//    float yOffset = yPos - lastY;
//
//    lastX = xPos;
//    lastY = yPos;
//
//    camera.processMouseMovement(xOffset, yOffset);
//}
//
//void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
//{
//    camera.processMouseScroll(static_cast<float>(yOffset));
//}
//
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//
//    camera.processKeyPresses(window, deltaTime);
//}
