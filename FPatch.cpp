#include "FPatch.h"
#include "ThreeChordSegment.h"

void FPatch::setUpBezierSurface(SmoothingViewer::ConstructionMode mode, size_t resolution, MyViewer::PolyMesh& mesh) {
	for (size_t i = 0; i < x_neighbours.size(); i++) {
		auto x_i = x_neighbours.at(i);
		internal_points.push_back(x_i->twist_opp);
	}
	if (x_neighbours.size() == 4) {	
		auto bezier = build4sidedPatch(mode);
		bezier->generateSurface(resolution, mesh);
	} else {
		auto beziers = centralSplit(mode);
		for (auto bezier : beziers) bezier->generateSurface(resolution, mesh);
	}
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
