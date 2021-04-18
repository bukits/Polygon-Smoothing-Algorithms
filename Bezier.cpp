#include "Bezier.h"

void Bezier::bernsteinAll(size_t n, double u, std::vector<double>& coeff) {
    coeff.clear(); coeff.reserve(n + 1);
    coeff.push_back(1.0);
    double u1 = 1.0 - u;
    for (size_t j = 1; j <= n; ++j) {
        double saved = 0.0;
        for (size_t k = 0; k < j; ++k) {
            double tmp = coeff[k];
            coeff[k] = saved + tmp * u1;
            saved = tmp * u;
        }
        coeff.push_back(saved);
    }
}

void Bezier::addControlPoint(MyViewer::MyTraits::Point cp) {
    Vec new_cp;
    new_cp.x = cp[0];
    new_cp.y = cp[1];
    new_cp.z = cp[2];
    this->addControlPoint(new_cp);
}

void Bezier::addControlPoint(Vec cp) {
    this->control_points.push_back(cp);
}

std::vector<Vec> Bezier::getCps() {
    return control_points;
}
