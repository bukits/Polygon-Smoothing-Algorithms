#pragma once

#include "Bezier.h"

class BezierSurface : public Bezier {
private:
	size_t degree[2];
	std::vector<std::vector<MyViewer::MyTraits::Point>> face_points;
	std::vector<MyViewer::MyTraits::Point> handles;
public:
	BezierSurface(size_t n, size_t m) {
		degree[0] = n;
		degree[1] = m;
	}

	void generateSurface(size_t resolution, std::vector<MyViewer::MyTraits::Point>&);
	size_t* getDegree();
	void colorSurface(Vec color);
	void colorSurfaceNames();
};