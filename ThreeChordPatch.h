#pragma once

#include "SmoothingViewer.h"

class ThreeChordPatch {
private:
	MyViewer::MyTraits::Point three_chord_cps[4][4];
	std::vector<MyViewer::MyTraits::Point> cps;

	MyViewer::MyTraits::Point translatePointBy(float rate, MyViewer::MyTraits::Point p1, MyViewer::MyTraits::Point p2);
public:
	BezierSurface* generatePatch(SmoothingViewer::ConstructionMode curve_mode);

	void drawControlNet();
	void drawWithNames();

	void setControlPoint(size_t i, size_t j, MyViewer::MyTraits::Point point);

	MyViewer::MyTraits::Point getControlPoint(int i, int j);
};