#pragma once

#include "SmoothingViewer.h"

class ThreeChordSegment {
private:
	MyViewer::MyTraits::Point three_chord_cps[4];
	MyViewer::MyTraits::Point half_point;

	MyViewer::MyTraits::Point left_segment[4];
	MyViewer::MyTraits::Point right_segment[4];
public:
	ThreeChordSegment(
		MyViewer::MyTraits::Point p1,
		MyViewer::MyTraits::Point p2,
		MyViewer::MyTraits::Point p3,
		MyViewer::MyTraits::Point p4) {
		three_chord_cps[0] = p1; three_chord_cps[1] = p2; three_chord_cps[2] = p3; three_chord_cps[3] = p4;

		half_point = 0.5 * (three_chord_cps[1] + three_chord_cps[2]);
	}

	MyViewer::MyTraits::Point translatePointBy(float rate, MyViewer::MyTraits::Point p1, MyViewer::MyTraits::Point p2);

	BezierCurve* buildCurve(SmoothingViewer::ConstructionMode curve_mode);

	MyViewer::MyTraits::Point getThreeChordCp(int index);
	MyViewer::MyTraits::Point getHalfPoint();

	MyViewer::MyTraits::Point getLeftSegmentPoint(int at);
	MyViewer::MyTraits::Point getRightSegmentPoint(int at);
};