// Test06.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Use GLEW for OpenGL function loading
#include <GL/glew.h>

// GLFW_INCLUDE_NONE ensures that GLFW doesn't load any OpenGL headers
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// Include GLM for math operations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>

// window size
const int max_window_width = 600;
const int max_window_height = 600;
const int standard_frame_rate = 300;
const GLfloat PI = 3.141592653f;

// GLFW event callback functions
void SetErrorCallback(int error, const char* description);
void SetFramebufferSizeCallback(GLFWwindow* window, int width, int height);
void SetFramebufferSize(float max_window_width, float max_window_height);
void SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void SetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

// Shader error checking functions
void CheckShaderCompileError(GLuint shader);
void CheckProgramLinkError(GLuint program);

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        return -1;
    }

    // Set GLFW error callback
    glfwSetErrorCallback(SetErrorCallback);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* glWindow = glfwCreateWindow(max_window_width, max_window_height, "OpenGL Demo 0.1", NULL, NULL);
    if (!glWindow)
    {
        glfwTerminate();
        return -1;
    }

    // Set framebuffer size callback
    glfwSetFramebufferSizeCallback(glWindow, SetFramebufferSizeCallback);
    // Set key callback
    glfwSetKeyCallback(glWindow, SetKeyCallback);
    // Set mouse button callback
    glfwSetMouseButtonCallback(glWindow, SetMouseButtonCallback);

    // Make the window's context current
    glfwMakeContextCurrent(glWindow);

    // Initialize GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    // Reset window size
    SetFramebufferSize(max_window_width, max_window_height);

    // Vertex Shader source code
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos, 0.0, 1.0);\n"
        "}\0";

    // Fragment Shader source code
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(0.75f, 0.75f, 0.75f, 1.0f);\n" // Gray color
        "}\n\0";

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    CheckShaderCompileError(vertexShader);

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    CheckShaderCompileError(fragmentShader);

    // Link shaders into a program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    CheckProgramLinkError(shaderProgram);

    // Delete the shader objects after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create VAO and VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Allocate space in VBO for large and small circle points
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 300000, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Store points on the circle
    std::vector<glm::vec2> bigCirclePoints;
    std::vector<glm::vec2> smallCirclePoints;

    // Circle properties
    float bigCircleAngle = 0.0f;
    float bigCircleRadius = 0.6f;
    float smallCircleAngle = 0.0f;
    float smallCircleRadius = 0.25f;

    // Coordinates
    float bigCircleX = 0, bigCircleY = 0;
    float smallCircleX = 0, smallCircleY = 0;

    // Timing variables
    double timePrevious = 0;
    double timeLast = 0;
    double frameTimeLimit = 0.01;
    double timeFramePrevious = -1;
    double timeFrameLast = 0;
    int fps = 0;

    // Main loop
    while (!glfwWindowShouldClose(glWindow))
    {
        timeFramePrevious = glfwGetTime();
        if ((timeFramePrevious - timeFrameLast) >= 1.0f)
        {
            std::cout << "当前帧率: " << fps << std::endl;
            std::cout << "跳帧时间: " << frameTimeLimit << std::endl;

            timeFrameLast = timeFramePrevious;

            if (fps > standard_frame_rate)
            {
                frameTimeLimit += frameTimeLimit * 0.1f;
            }
            else if (fps < standard_frame_rate)
            {
                frameTimeLimit -= frameTimeLimit * 0.1f;
            }
            fps = 0;

            if (frameTimeLimit < 0.000001)
            {
                frameTimeLimit = 0.1;
            }
        }

        timePrevious = timeFramePrevious;
        if ((timePrevious - timeLast) < frameTimeLimit)
        {
            continue;
        }
        timeLast = timePrevious;
        fps += 1;

        // Clear the screen
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Calculate new points for the big circle
        bigCircleX = bigCircleRadius * cos(glm::radians(bigCircleAngle));
        bigCircleY = bigCircleRadius * sin(glm::radians(bigCircleAngle));
        bigCirclePoints.push_back(glm::vec2(bigCircleX, bigCircleY));

        for (int i = 0; i < 4; i++)
        {
            if (smallCircleAngle <= 360)
            {
                smallCircleAngle += 0.33f;
            }
            else
            {
                smallCircleAngle = 0;
            }
            smallCircleX = bigCircleX + smallCircleRadius * cos(glm::radians(smallCircleAngle));
            smallCircleY = bigCircleY + smallCircleRadius * sin(glm::radians(smallCircleAngle));
            smallCirclePoints.push_back(glm::vec2(smallCircleX, smallCircleY));
        }

        // Use the shader program
        glUseProgram(shaderProgram);

        // Update VBO data for big circle
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, bigCirclePoints.size() * sizeof(glm::vec2), &bigCirclePoints[0]);

        // Draw big circle
        glPointSize(1.0f);
        glDrawArrays(GL_POINTS, 0, bigCirclePoints.size());

        // Update VBO data for small circle
        glBufferSubData(GL_ARRAY_BUFFER, bigCirclePoints.size() * sizeof(glm::vec2), smallCirclePoints.size() * sizeof(glm::vec2), &smallCirclePoints[0]);

        // Draw small circle
        glPointSize(2.0f);
        glDrawArrays(GL_POINTS, bigCirclePoints.size(), smallCirclePoints.size());

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Update the angle
        bigCircleAngle += 0.5f;
        if (bigCircleAngle >= 360.0f)
        {
            bigCircleAngle = 0.0f;
            bigCirclePoints.clear();
        }

        glfwSwapBuffers(glWindow);
        glfwPollEvents();
    }

    // Clean up resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(glWindow);
    glfwTerminate();

    return 0;
}

// Shader error checking functions
void CheckShaderCompileError(GLuint shader)
{
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void CheckProgramLinkError(GLuint program)
{
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

// GLFW callback functions
void SetFramebufferSize(float window_width, float window_height)
{
    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void SetErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

void SetFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    SetFramebufferSize(width, height);
}

void SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void SetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}
