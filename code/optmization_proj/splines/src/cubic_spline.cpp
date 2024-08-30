#include "PointPathRenderer.hpp"
#include "cubic_spline.hpp"

int main() {
    try {
        PointPathRenderer renderer(800, 600, "Point Path Renderer");

        // Example points forming a path
        std::vector<float> points = {
            100.0f, 100.0f,
            200.0f, 200.0f,
            300.0f, 150.0f,
            400.0f, 250.0f,
            500.0f, 200.0f
        };

        // Set line color to red
        renderer.setLineColor(1.0f, 0.0f, 0.0f);

        // Set line width to 3.0
        renderer.setLineWidth(3.0f);

        renderer.render(points);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
