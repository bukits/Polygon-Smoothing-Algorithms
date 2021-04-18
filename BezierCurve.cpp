#include "BezierCurve.h"

std::vector<MyViewer::MyTraits::Point> BezierCurve::generateCurve(size_t resolution) {
    std::vector<MyViewer::MyTraits::Point> points;
    std::vector<double> coeff_u;
    size_t n = control_points.size() - 1;

    for (size_t i = 0; i < resolution; ++i) {
        double u = (double)i / (double)(resolution - 1);
        bernsteinAll(n, u, coeff_u);
        Vec p(0.0, 0.0, 0.0);
        for (size_t k = 0, index = 0; k <= n; ++k, ++index)
            p += control_points[index] * coeff_u[k];
        points.push_back(MyViewer::Vector(static_cast<double*>(p)));
    }
    return points;
}