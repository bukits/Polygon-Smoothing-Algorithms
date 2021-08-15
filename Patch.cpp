#include "Patch.h"
#include "BezierSurface.h"
#include "ThreeChordSegment.h"
#include "ThreeChordPatch.h"

void Patch::build4sidedPatch(SmoothingViewer::ConstructionMode mode) {
	ThreeChordPatch* three_chord_patch = new ThreeChordPatch();
	for (int i = 0; i < 4; ++i) three_chord_patch->setControlPoint(0, i, bounding_curves.at(0)->getThreeChordCp(i));
	for (int j = 0; j < 4; ++j) three_chord_patch->setControlPoint(j, 3, bounding_curves.at(1)->getThreeChordCp(j));
	for (int i = 3; i >= 0; --i) three_chord_patch->setControlPoint(3, i, bounding_curves.at(2)->getThreeChordCp(3 - i));
	for (int j = 3; j >= 0; --j) three_chord_patch->setControlPoint(j, 0, bounding_curves.at(3)->getThreeChordCp(3 - j));

	three_chord_patch->setControlPoint(1, 1, internal_points.at(0));
	three_chord_patch->setControlPoint(1, 2, internal_points.at(1));
	three_chord_patch->setControlPoint(2, 2, internal_points.at(2));
	three_chord_patch->setControlPoint(2, 1, internal_points.at(3));

	three_chord_patches.push_back(three_chord_patch);
	auto bezier = three_chord_patch->generatePatch(mode);
	bezier_patches.push_back(bezier);
}

MyViewer::MyTraits::Point Patch::projectionToPlane(MyViewer::MyTraits::Point twist, OpenMesh::Vec3f face_normal) {
	auto v = twist - central_point;
	auto d = face_normal.dot(v);
	auto twist_new = twist - d * face_normal;
	return twist_new;
}

OpenMesh::Vec3f Patch::calculateBestFaceNormal() {
	OpenMesh::Vec3f n_c;
	n_c[0] = n_c[1] = n_c[2] = 0.0;
	for (size_t i = 0; i < twist_points.size(); i++) {
		OpenMesh::Vec3f v_i, v_i_next;
		if (i == twist_points.size() - 1) {
			v_i = central_point - twist_points.at(i);
			v_i_next = central_point - twist_points.at(0);
		}
		else {
			v_i = central_point - twist_points.at(i);
			v_i_next = central_point - twist_points.at(i + 1);
		}
		auto n_i = v_i.cross(v_i_next);
		n_c += n_i;
	}
	n_c /= twist_points.size();
	return n_c.normalize();
}

void Patch::attachCurveToX(ThreeChordSegment* curve, XObject* x) {
	auto it = x->connected_curves.find(curve->getHalfPoint());
	if (it == x->connected_curves.end())
		x->connected_curves.insert({ curve->getHalfPoint(), curve });
}

ThreeChordSegment* Patch::searchCommonCurve(XObject* x1, XObject* x2) {
	for (auto half_curve : x1->connected_curves) {
		auto it = x2->connected_curves.find(half_curve.first);
		if (it != x2->connected_curves.end())
			return half_curve.second;
	}
}

MyViewer::MyTraits::Point Patch::translatePointBy(float rate, MyViewer::MyTraits::Point p1, MyViewer::MyTraits::Point p2) {
	auto dir = p1 - p2;
	return rate * dir.length() * dir.normalize();
}

MyViewer::MyTraits::Point Patch::bezierToThreeChord(MyViewer::MyTraits::Point q1, MyViewer::MyTraits::Point q2, MyViewer::MyTraits::Point p0) {
	auto half = 0.5 * (q1 + q2);
	auto p = p0 + translatePointBy(0.75, half, p0);
	return p;
}

void Patch::reverseNeighbour() {
	std::reverse(x_neighbours.begin(), x_neighbours.end());
}

bool Patch::containsNeighbour(XObject* xObj) {
	for (auto x : x_neighbours)
		if (xObj->x == x->x)
			return true;
	return false;
}

bool Patch::isNext(XObject* x_1, XObject* x_2) {
	size_t n_1, n_2 = 0;
	for (size_t i = 0; i < x_neighbours.size(); ++i) {
		if (x_neighbours.at(i)->x == x_1->x) n_1 = i;
		if (x_neighbours.at(i)->x == x_2->x) n_2 = i;
	}

	return n_2 > n_1 || (n_2 == 0 && n_1 == x_neighbours.size() - 1);
}

std::vector<XObject*> Patch::getNeighbours() {
	return x_neighbours;
}

void Patch::caculateInternalBezier(OpenMesh::Vec3f normal) {
	for (size_t i = 0; i < x_neighbours.size(); i++) {
		auto x = x_neighbours.at(i);
		const auto& t = internal_points.at(i);
		auto t_after_projection = projectionToPlane(t, normal);
		ThreeChordSegment* left_curve, * right_curve;
		XObject* x_next, * x_back;
		if (i == 0) {
			x_next = x_neighbours.at(i + 1);
			x_back = x_neighbours.at(x_neighbours.size() - 1);
		}
		else if (i == x_neighbours.size() - 1) {
			x_next = x_neighbours.at(0);
			x_back = x_neighbours.at(i - 1);
		}
		else {
			x_next = x_neighbours.at(i + 1);
			x_back = x_neighbours.at(i - 1);
		}
		right_curve = searchCommonCurve(x, x_back);
		left_curve = searchCommonCurve(x, x_next);

		auto t_bezier = t_after_projection + translatePointBy(1 / 3.0f, t_after_projection, left_curve->getThreeChordCp(1))
			+ translatePointBy(1 / 3.0f, t_after_projection, right_curve->getThreeChordCp(2));
		internal_bezier_points.push_back(t_bezier);
	}
}

void Patch::centralSplit(SmoothingViewer::ConstructionMode mode) {
	MyViewer::MyTraits::Point pointSum;
	pointSum[0] = pointSum[1] = pointSum[2] = 0.0;
	for (const auto& p : internal_points) {
		twist_points.push_back(p);
		pointSum += p;
	}
	central_point = pointSum / internal_points.size();

	auto face_normal = calculateBestFaceNormal();
	caculateInternalBezier(face_normal);

	for (size_t i = 0; i < x_neighbours.size(); i++){
		auto x = x_neighbours.at(i);
		const auto& t = internal_points.at(i);
		auto t_after_projection = projectionToPlane(t, face_normal);
		projected_twist_points.push_back(t_after_projection);
		auto t_new = 0.5 * (t_after_projection + central_point);
		ThreeChordSegment* left_curve, *right_curve;
		XObject *x_next, *x_back;
		int next, back;
		if (i == 0) {
			x_next = x_neighbours.at(i + 1);
			x_back = x_neighbours.at(x_neighbours.size() - 1);
			next = i + 1;
			back = x_neighbours.size() - 1;
		} else if (i == x_neighbours.size() - 1) {
			x_next = x_neighbours.at(0);
			x_back = x_neighbours.at(i - 1);
			next = 0;
			back = i - 1;
		} else {
			x_next = x_neighbours.at(i + 1);
			x_back = x_neighbours.at(i - 1);
			next = i + 1;
			back = i - 1;
		}
		right_curve = searchCommonCurve(x, x_back);
		left_curve = searchCommonCurve(x, x_next);
		auto t_next = projectionToPlane(internal_points.at(next), face_normal);
		auto t_back = projectionToPlane(internal_points.at(back), face_normal);
		auto t_next_new = 0.5 * (central_point + t_next);
		auto t_back_new = 0.5 * (central_point + t_back);
		auto w_next = 0.5 * (t_next + t_after_projection);
		auto w_back = 0.5 * (t_back + t_after_projection);
		auto w_next_new = 0.5 * (t_new + t_next_new);
		auto w_back_new = 0.5 * (t_new + t_back_new);

		ThreeChordPatch* three_chord_patch = new ThreeChordPatch();
		for (int i = 0; i < 4; ++i) three_chord_patch->setControlPoint(i, 0, left_curve->getLeftSegmentPoint(i));
		for (int j = 0; j < 4; ++j) three_chord_patch->setControlPoint(0, j, right_curve->getRightSegmentPoint(j));
		three_chord_patch->setControlPoint(3, 3, central_point);
		three_chord_patch->setControlPoint(3, 2, w_next_new);
		three_chord_patch->setControlPoint(2, 2, t_new);
		three_chord_patch->setControlPoint(2, 3, w_back_new);

		auto p11 = 0.25 * (t_after_projection + left_curve->getThreeChordCp(1) + right_curve->getThreeChordCp(2) + left_curve->getThreeChordCp(0));
		auto p13 = 0.25 * (t_after_projection + t_back + right_curve->getThreeChordCp(1) + right_curve->getThreeChordCp(2));
		auto p31 = 0.25 * (t_after_projection + t_next + left_curve->getThreeChordCp(1) + left_curve->getThreeChordCp(2));

		MyViewer::MyTraits::Point p12, p21;
		if (mode == SmoothingViewer::ConstructionMode::CUBIC) {
			const auto& t_bezier = internal_bezier_points.at(i);
			const auto& t_next_bezier = internal_bezier_points.at(next);
			const auto& t_back_bezier = internal_bezier_points.at(back);

			auto q_i = 0.25 * (left_curve->getBezierCps(0) + left_curve->getBezierCps(1) + right_curve->getBezierCps(2) + t_bezier);
			auto q_next = 0.25 * (left_curve->getBezierCps(1) + left_curve->getBezierCps(2) + t_next_bezier + t_bezier);
			auto q_back = 0.25 * (right_curve->getBezierCps(1) + right_curve->getBezierCps(2) + t_back_bezier + t_bezier);
			p21 = bezierToThreeChord(q_i, q_next, left_curve->getHalfPoint());
			p12 = bezierToThreeChord(q_i, q_back, right_curve->getHalfPoint());
		}
		if (mode == SmoothingViewer::ConstructionMode::QUADRATIC) {
			p12 = 0.25 * (t_after_projection + right_curve->getThreeChordCp(2) + w_back + right_curve->getHalfPoint());
			p21 = 0.25 * (t_after_projection + left_curve->getThreeChordCp(1) + w_next + left_curve->getHalfPoint());
		}

		three_chord_patch->setControlPoint(1, 1, p11);
		three_chord_patch->setControlPoint(1, 2, p12);
		three_chord_patch->setControlPoint(1, 3, p13);
		three_chord_patch->setControlPoint(2, 1, p21);
		three_chord_patch->setControlPoint(3, 1, p31);

		three_chord_patches.push_back(three_chord_patch);
		auto bezier_surface = three_chord_patch->generatePatch(mode);
		bezier_patches.push_back(bezier_surface);
	}
}

void Patch::drawLineSplit(std::vector<MyViewer::MyTraits::Point> points, Vec color) {
	glDisable(GL_LIGHTING);
	glLineWidth(3.0);
	glColor3d(
		color.x,
		color.y,
		color.z);

	glBegin(GL_LINE_LOOP);
	for (auto point : points) {
		const Vec& p = Vec(point[0], point[1], point[2]);
		glVertex3dv(p);
	}
	glEnd();
	glLineWidth(1.0);
	glPointSize(8.0);
	glColor3d(
		color.x,
		color.y,
		color.z);
	glBegin(GL_POINTS);
	for (auto point : points) {
		const Vec& p = Vec(point[0], point[1], point[2]);
		glVertex3dv(p);
	}
	glEnd();
	glPointSize(1.0);
	glEnable(GL_LIGHTING);
}

void Patch::drawOriginalFace() {
	drawLineSplit(this->twist_points, Vec(1, 0, 1));
}

void Patch::drawBestFace() {
	drawLineSplit(this->projected_twist_points, Vec(1, 0, 0.5));
}

void Patch::drawWithNamesFace(std::vector<MyViewer::MyTraits::Point> points) {
	for (size_t i = 0, ie = points.size(); i < ie; ++i) {
		auto p = points[i];
		Vec const& p_vec = Vec(p[0], p[1], p[2]);
		glPushName(i);
		glRasterPos3fv(p_vec);
		glPopName();
	}
}

void Patch::drawWithNamesControl(bool isBezier, bool isThreeChord) {
	if (isThreeChord) {
		for (auto three_patch : three_chord_patches)
			three_patch->drawWithNames();
	}

	if (isBezier) {
		for (auto bezier : bezier_patches) {
			auto cps = bezier->getCps();
			for (size_t i = 0, ie = cps.size(); i < ie; ++i) {
				Vec const& p = cps[i];
				glPushName(i);
				glRasterPos3fv(p);
				glPopName();
			}
		}
	}
}

void Patch::drawWithNamesOriginalFace() {
	drawWithNamesFace(this->twist_points);
}

void Patch::drawWithNamesBestFace() {
	drawWithNamesFace(this->projected_twist_points);
}

void Patch::drawControlNet(bool isBezier, bool isThreeChord) const {
	if (isThreeChord) {
		for (auto three_patch : three_chord_patches)
			three_patch->drawControlNet();
	}
	if (isBezier) {
		auto color = color_control_net;
		glLineWidth(3.0);
		glColor3d(
			color.x,
			color.y,
			color.z);
		for (auto bezier : bezier_patches) {
			auto degree = bezier->getDegree();
			auto cps = bezier->getCps();
			size_t m = degree[1] + 1;
			for (size_t k = 0; k < 2; ++k)
				for (size_t i = 0; i <= degree[k]; ++i) {
					glBegin(GL_LINE_STRIP);
					for (size_t j = 0; j <= degree[1 - k]; ++j) {
						size_t const index = k ? j * m + i : i * m + j;
						const auto& p = cps[index];
						glVertex3dv(p);
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
			for (const auto& p : cps)
				glVertex3dv(p);
			glEnd();
			glPointSize(1.0);
		}
	}
}

void Patch::colorBezierSurfaces() {
	for (auto bezier : bezier_patches) {
		bezier->colorSurface(color_patch);
	}
}

void Patch::colorBezierSurfacesNames() {
	for (auto bezier : bezier_patches) {
		bezier->colorSurfaceNames();
	}
}

void Patch::setNeighbour(XObject *x) {
	x_neighbours.push_back(x);
}

MyViewer::MyTraits::Point Patch::findSharedE(XObject* x1, XObject* x2) {
	for (auto x1_e : x1->connected_e_points)
		for (auto x2_e : x2->connected_e_points)
			if (x1_e == x2_e)
				return x1_e;
}