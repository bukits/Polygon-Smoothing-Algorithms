#include "ThreeChordPatch.h"

BezierSurface* ThreeChordPatch::generatePatch(SmoothingViewer::ConstructionMode curve_mode) {
	BezierSurface* bezier;
	size_t n, m;

	for (size_t i = 0; i < 4; ++i)
		for (size_t j = 0; j < 4; ++j)
			cps.push_back(three_chord_cps[i][j]);

	if (curve_mode == SmoothingViewer::ConstructionMode::QUADRATIC) {
		n = 2; m = 2;
		bezier = new BezierSurface(n, m);
		bezier->addControlPoint(three_chord_cps[0][0]);
		bezier->addControlPoint(three_chord_cps[0][1] + translatePointBy(1, three_chord_cps[0][1], three_chord_cps[0][0]));
		bezier->addControlPoint(three_chord_cps[0][3]);
		
		bezier->addControlPoint(three_chord_cps[1][0] + translatePointBy(1, three_chord_cps[1][0], three_chord_cps[0][0]));
		bezier->addControlPoint(three_chord_cps[1][1] + translatePointBy(1, three_chord_cps[1][1], three_chord_cps[1][0]) + translatePointBy(1, three_chord_cps[1][1], three_chord_cps[0][1]));
		bezier->addControlPoint(three_chord_cps[1][3] + translatePointBy(1, three_chord_cps[1][3], three_chord_cps[0][3]));

		bezier->addControlPoint(three_chord_cps[3][0]);
		bezier->addControlPoint(three_chord_cps[3][1] + translatePointBy(1, three_chord_cps[3][1], three_chord_cps[3][0]));
		bezier->addControlPoint(three_chord_cps[3][3]);
		return bezier;
	} else if (curve_mode == SmoothingViewer::ConstructionMode::CUBIC) {
		n = 3; m = 3;
		bezier = new BezierSurface(n, m);
		bezier->addControlPoint(three_chord_cps[0][0]);
		bezier->addControlPoint(three_chord_cps[0][1] + translatePointBy(1 / 3.0f, three_chord_cps[0][1], three_chord_cps[0][0]));
		bezier->addControlPoint(three_chord_cps[0][2] + translatePointBy(1 / 3.0f, three_chord_cps[0][2], three_chord_cps[0][3]));
		bezier->addControlPoint(three_chord_cps[0][3]);

		bezier->addControlPoint(three_chord_cps[1][0] + translatePointBy(1 / 3.0f, three_chord_cps[1][0], three_chord_cps[0][0]));
		bezier->addControlPoint(three_chord_cps[1][1] + translatePointBy(1 / 3.0f, three_chord_cps[1][1], three_chord_cps[1][0]) + translatePointBy(1 / 3.0f, three_chord_cps[1][1], three_chord_cps[0][1]));
		bezier->addControlPoint(three_chord_cps[1][2] + translatePointBy(1 / 3.0f, three_chord_cps[1][2], three_chord_cps[0][2]) + translatePointBy(1 / 3.0f, three_chord_cps[1][2], three_chord_cps[1][3]));
		bezier->addControlPoint(three_chord_cps[1][3] + translatePointBy(1 / 3.0f, three_chord_cps[1][3], three_chord_cps[0][3]));

		bezier->addControlPoint(three_chord_cps[2][0] + translatePointBy(1 / 3.0f, three_chord_cps[2][0], three_chord_cps[3][0]));
		bezier->addControlPoint(three_chord_cps[2][1] + translatePointBy(1 / 3.0f, three_chord_cps[2][1], three_chord_cps[2][0]) + translatePointBy(1 / 3.0f, three_chord_cps[2][1], three_chord_cps[3][1]));
		bezier->addControlPoint(three_chord_cps[2][2] + translatePointBy(1 / 3.0f, three_chord_cps[2][2], three_chord_cps[2][3]) + translatePointBy(1 / 3.0f, three_chord_cps[2][2], three_chord_cps[3][2]));
		bezier->addControlPoint(three_chord_cps[2][3] + translatePointBy(1 / 3.0f, three_chord_cps[2][3], three_chord_cps[3][3]));

		bezier->addControlPoint(three_chord_cps[3][0]);
		bezier->addControlPoint(three_chord_cps[3][1] + translatePointBy(1 / 3.0f, three_chord_cps[3][1], three_chord_cps[3][0]));
		bezier->addControlPoint(three_chord_cps[3][2] + translatePointBy(1 / 3.0f, three_chord_cps[3][2], three_chord_cps[3][3]));
		bezier->addControlPoint(three_chord_cps[3][3]);
		return bezier;
	}
}

void ThreeChordPatch::drawWithNames() {
	for (size_t i = 0, ie = cps.size(); i < ie; ++i) {
		auto p = cps[i];
		Vec const& p_vec = Vec(p[0], p[1], p[2]);
		glPushName(i);
		glRasterPos3fv(p_vec);
		glPopName();
	}
}

void ThreeChordPatch::drawControlNet() {
	auto color = Vec(0.0, 1.0, 0.5);
	glLineWidth(3.0);
	glColor3d(
		color.x,
		color.y,
		color.z);
	auto degree = 3;
	size_t m = 4;
	for (size_t k = 0; k < 2; ++k)
		for (size_t i = 0; i <= degree; ++i) {
			glBegin(GL_LINE_STRIP);
			for (size_t j = 0; j <= degree; ++j) {
				size_t const index = k ? j * m + i : i * m + j;
				const auto& p = cps[index];
				const auto& p_vec = Vec(p[0], p[1], p[2]);
				glVertex3dv(p_vec);
			}
			glEnd();
		}
	glLineWidth(1.0);
	glPointSize(8.0);
	glColor3d(
		color.x,
		color.y,
		color.z);
	glBegin(GL_POINTS);
	for (const auto& p : cps) {
		auto p_vec = Vec(p[0], p[1], p[2]);
		glVertex3dv(p_vec);
	}
	glEnd();
	glPointSize(1.0);
}

MyViewer::MyTraits::Point ThreeChordPatch::translatePointBy(float rate, MyViewer::MyTraits::Point p1, MyViewer::MyTraits::Point p2) {
	auto dir = p1 - p2;
	return rate * dir.length() * dir.normalize();
}

void ThreeChordPatch::setControlPoint(size_t i, size_t j, MyViewer::MyTraits::Point point) {
	three_chord_cps[i][j] = point;
}

MyViewer::MyTraits::Point ThreeChordPatch::getControlPoint(int i, int j) {
	return three_chord_cps[i][j];
}