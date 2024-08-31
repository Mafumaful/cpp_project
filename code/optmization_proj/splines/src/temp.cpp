#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "cubic_spline.hpp"  // Assuming this contains the CubicSpline class
#include "cubic_curve.hpp"   // Assuming this contains the CubicCurve class

class CubicSplineRenderer {
public:
    CubicSplineRenderer(int width, int height, const std::string& title)
        : windowWidth(width), windowHeight(height), windowTitle(title) {
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
        setupViewport(windowWidth, windowHeight);
    }

    ~CubicSplineRenderer() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void renderSpline(const CubicCurve& cubicCurve, double t_start, double t_end, int num_samples) {
        std::vector<Eigen::Vector2d> spline_points;
        double step = (t_end - t_start) / (num_samples - 1);

        for (int i = 0; i < num_samples; ++i) {
            double t = t_start + i * step;
            Eigen::Vector2d pos = cubicCurve.getPos(t);
            spline_points.push_back(pos);
        }

        render(spline_points);
    }

private:
    int windowWidth;
    int windowHeight;
    std::string windowTitle;
    GLFWwindow* window;

    void setupViewport(int width, int height) {
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, 0, height, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void render(const std::vector<Eigen::Vector2d>& spline_points) {
        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);

            glColor3f(0.0f, 0.0f, 0.0f);
            glLineWidth(2.0f);
            glBegin(GL_LINE_STRIP);
            for (const auto& point : spline_points) {
                glVertex2f(point.x(), point.y());
            }
            glEnd();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
};

int main() {
    try {
        // Define the number of segments and the coefficients for each cubic polynomial segment
        std::vector<double> durations = {1.0, 1.0}; // Duration of each segment
        std::vector<Eigen::Matrix<double, 2, 4>> coeffMats(2);

        // Define the coefficients for the first segment
        coeffMats[0] << 1, 2, 1, 0.5,
                        2, 1, 3, 1;

        // Define the coefficients for the second segment
        coeffMats[1] << -1, 0.5, -2, 0.1,
                        3, 2, 1, 0.2;

        // Create the cubic curve
        CubicCurve cubicCurve(durations, coeffMats);

        CubicSplineRenderer renderer(800, 600, "Cubic Spline Renderer");

        // Render the cubic spline from t = 0.0 to t = 2.0 with 100 samples
        renderer.renderSpline(cubicCurve, 0.0, 2.0, 100);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
