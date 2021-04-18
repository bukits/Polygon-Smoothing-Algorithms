#pragma once

#include "MyViewer.h"

class Bezier {
protected:
	std::vector<Vec> control_points;

	void bernsteinAll(size_t n, double u, std::vector<double>& coeff);
public:
	void addControlPoint(Vec cp);
	void addControlPoint(MyViewer::MyTraits::Point cp);
	std::vector<Vec> getCps();
};