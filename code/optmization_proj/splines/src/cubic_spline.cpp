#include "PointPathRenderer.hpp"
#include "cubic_spline.hpp"

int main() {
    try {
        PointPathRenderer renderer(800, 600, "Point Path Renderer");

        // Example points forming a path
        std::vector<double> points = {
            100.0d, 100.0d,
            200.0d, 200.0d,
            300.0d, 150.0d,
            400.0d, 250.0d,
            500.0d, 200.0d
        };

        Eigen::VectorXd eigen_vec = Eigen::Map<Eigen::VectorXd>(points.data(), points.size());

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
