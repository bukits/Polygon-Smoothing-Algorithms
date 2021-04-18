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

		if (/*quadratic*/ true) {
			half_point = 0.5 * (three_chord_cps[1] + three_chord_cps[2]);
		}
		else if (/*cubic*/ false) {
		}

		left_segment[0] = three_chord_cps[0];
		left_segment[1] = (three_chord_cps[0] + three_chord_cps[1]) * 0.5;
		left_segment[2] = (three_chord_cps[1] + half_point) * 0.5;
		left_segment[3] = half_point;

		right_segment[0] = three_chord_cps[3];
		right_segment[1] = (three_chord_cps[2] + three_chord_cps[3]) * 0.5;
		right_segment[2] = (three_chord_cps[2] + half_point) * 0.5;
		right_segment[3] = half_point;
	}

	BezierCurve* generateCurve(SmoothingViewer::ConstructionMode curve_mode);

	MyViewer::MyTraits::Point getThreeChordCp(int index);
	MyViewer::MyTraits::Point getHalfPoint();

	MyViewer::MyTraits::Point getLeftSegmentPoint(int at);
	MyViewer::MyTraits::Point getRightSegmentPoint(int at);
};