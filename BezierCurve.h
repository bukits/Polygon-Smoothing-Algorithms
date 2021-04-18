#pragma once
#include "Bezier.h"

class BezierCurve : public Bezier {
private:
	std::vector<MyViewer::MyTraits::Point> body_points;

public:
	std::vector<MyViewer::MyTraits::Point> generateCurve(size_t resolution);
};