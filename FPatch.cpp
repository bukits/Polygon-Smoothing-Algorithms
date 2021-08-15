#include "FPatch.h"
#include "ThreeChordSegment.h"

void FPatch::setUpBezierSurface(SmoothingViewer::ConstructionMode mode) {
	for (size_t i = 0; i < x_neighbours.size(); i++) {
		auto x_i = x_neighbours.at(i);
		internal_points.push_back(x_i->twist_opp);
	}
	if (x_neighbours.size() == 4) build4sidedPatch(mode);
	else centralSplit(mode);
}

void FPatch::generateBezierSurface(size_t resolution, std::vector<MyViewer::MyTraits::Point>& vertices) {

	if (x_neighbours.size() == 4) {
		resolution = ((resolution - 1) * 2) + 1;
	}

	for (auto bezier : bezier_patches)
		bezier->generateSurface(resolution, vertices);
}

std::vector<ThreeChordSegment*> FPatch::setUpBoundingCurves() {
	for (size_t i = 0; i < x_neighbours.size(); i++) {
		auto x_i = x_neighbours.at(i);
		XObject *x_i_next;
		if (i == x_neighbours.size() - 1) {
			x_i_next = x_neighbours.at(0);
		}
		else {
			x_i_next = x_neighbours.at(i + 1);
		}

		auto e = findSharedE(x_i, x_i_next);

		auto tangent1 = x_i->ePoint_tangentOpposite.find(e)->second;
		auto tangent2 = x_i_next->ePoint_tangentOpposite.find(e)->second;

		auto curve = new ThreeChordSegment(
			x_i->x,
			tangent1,
			tangent2,
			x_i_next->x);
		attachCurveToX(curve, x_i);
		attachCurveToX(curve, x_i_next);
		bounding_curves.push_back(curve);
	}
	return bounding_curves;
}
