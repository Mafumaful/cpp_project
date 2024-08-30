#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>

class PointPathRenderer {
public:
    PointPathRenderer(int width, int height, const std::string& title)
        : windowWidth(width), windowHeight(height), windowTitle(title), lineWidth(1.0f) {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);
        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);

        if (glewInit() != GLEW_OK) {
            glfwDestroyWindow(window);
            glfwTerminate();
            throw std::runtime_error("Failed to initialize GLEW");
        }

        // Set the background color to white
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // Setup viewport and projection matrix
        setupViewport(windowWidth, windowHeight);

        // Register framebuffer size callback
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
        glfwSetWindowUserPointer(window, this);
    }

    ~PointPathRenderer() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void setLineColor(float r, float g, float b) {
        lineColor[0] = r;
        lineColor[1] = g;
        lineColor[2] = b;
    }

    void setLineWidth(float width) {
        lineWidth = width;
    }

    void render(const std::vector<float>& points) {
        if (points.size() % 2 != 0) {
            throw std::runtime_error("Points vector size must be even.");
        }

        this->points = points;
        calculateOffsets();

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);

            // Set line width
            glLineWidth(lineWidth);

            // Draw the path from points, applying translation for centering
            glColor3f(lineColor[0], lineColor[1], lineColor[2]); // Set the path color
            glBegin(GL_LINE_STRIP);
            for (size_t i = 0; i < points.size(); i += 2) {
                glVertex2f(points[i] + offsetX, points[i + 1] + offsetY);
            }
            glEnd();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

private:
    int windowWidth;
    int windowHeight;
    std::string windowTitle;
    GLFWwindow* window;
    float lineColor[3] = {0.0f, 0.0f, 0.0f}; // Default color: black
    float lineWidth; // Line width
    float offsetX = 0.0f, offsetY = 0.0f;
    std::vector<float> points;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        PointPathRenderer* renderer = static_cast<PointPathRenderer*>(glfwGetWindowUserPointer(window));
        renderer->windowWidth = width;
        renderer->windowHeight = height;
        renderer->setupViewport(width, height);
        renderer->calculateOffsets();
    }

    void setupViewport(int width, int height) {
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, 0, height, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void calculateOffsets() {
        if (points.empty()) return;

        // Calculate bounding box
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();

        for (size_t i = 0; i < points.size(); i += 2) {
            minX = std::min(minX, points[i]);
            maxX = std::max(maxX, points[i]);
            minY = std::min(minY, points[i + 1]);
            maxY = std::max(maxY, points[i + 1]);
        }

        // Calculate the translation needed to center the shape
        offsetX = (windowWidth - (maxX - minX)) / 2.0f - minX;
        offsetY = (windowHeight - (maxY - minY)) / 2.0f - minY;
    }
};