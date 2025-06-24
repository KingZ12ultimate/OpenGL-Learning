#include "App.hpp"
#include "stb_image.h"

namespace App
{
    GLFWwindow* window = nullptr;
    bool firstMouse = true;
    bool initialized = false;
    float lastX = 0.0f, lastY = 0.0f;
    unsigned int SCREEN_WIDTH = 1080, SCREEN_HEIGHT = 720;
    float near = 0.1f, far = 1000.0f;
    Camera camera(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f), 45.0f, near, far);
    ImGuiIO* io = nullptr;

    void Initialize(unsigned int screenWidth, unsigned int screenHeight, const char* title)
    {
        if (initialized)
        {
            std::cout << "A running instance already exists!" << std::endl;
            return;
        }
        initialized = true;

        SCREEN_WIDTH = screenWidth;
        SCREEN_HEIGHT = screenHeight;

        firstMouse = true;
        lastX = (float)SCREEN_WIDTH / 2.0f;
        lastY = (float)SCREEN_HEIGHT / 2.0f;

        #pragma region Graphics initialization
        // initialize GLFW
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
        {
            std::cout << "Failed to initialize GLFW!" << std::endl;
            return;
        }

        const char* glslVersion = "#version 460";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        // create the window
        float mainScale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
        window = glfwCreateWindow(screenWidth, screenHeight, title, NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // setup Dear ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = &ImGui::GetIO();
        io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(mainScale);
        style.FontScaleDpi = mainScale;
        io->ConfigDpiScaleFonts = true;
        io->ConfigDpiScaleViewports = true;

        if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // setup platform / renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glslVersion);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }

        // Check for OpenGL errors
        GLenum err = 0;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            std::cout << "OpenGL error: " << err << std::endl;
        }

        stbi_set_flip_vertically_on_load(true);
        #pragma endregion
    }

    void Clean()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // Sets the viewport size on the window that OpenGL will render to.
        glViewport(0, 0, width, height);
    }

    void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
    {
        if (io->WantCaptureMouse) return;

        float xPos = static_cast<float>(xPosIn);
        float yPos = static_cast<float>(yPosIn);

        if (firstMouse)
        {
            lastX = xPos;
            lastY = yPos;
            firstMouse = false;
        }

        float xOffset = xPos - lastX;
        float yOffset = yPos - lastY;

        lastX = xPos;
        lastY = yPos;

        camera.processMouseMovement(xOffset, yOffset);
    }

    void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
    {
        if (!io->WantCaptureMouse)
            camera.processMouseScroll(static_cast<float>(yOffset));
    }
}
