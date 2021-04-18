#include "EPatch.h"
#include "ThreeChordSegment.h"

std::vector<BezierSurface*> EPatch::setUpBezierSurface(SmoothingViewer::ConstructionMode mode) {
	for (size_t i = 0; i < x_neighbours.size(); i++) {
		auto x_i = x_neighbours.at(i);
		auto twist_i = x_i->ePoint_twist_i.find(common_edge)->second;
		internal_points.push_back(twist_i);
	}
	return build4sidedPatch(mode);
}

std::vector<ThreeChordSegment*> EPatch::setUpBoundingCurves() {
	common_edge = findSharedE(x_neighbours.at(0), x_neighbours.at(1));
	for (size_t i = 0; i < x_neighbours.size(); i++) {
		auto x_i = x_neighbours.at(i);
		XObject *x_i_next;
		if (i == x_neighbours.size() - 1) {
			x_i_next = x_neighbours.at(0);
		} else {
			x_i_next = x_neighbours.at(i + 1);
		}

		MyViewer::MyTraits::Point tangent1, tangent2;
		if (i % 2 == 0) {
			tangent1 = x_i->ePoint_tangentOpposite.find(common_edge)->second;
			tangent2 = x_i_next->ePoint_tangentOpposite.find(common_edge)->second;
		} else {
			tangent1 = x_i->ePoint_tangent.find(common_edge)->second;
			tangent2 = x_i_next->ePoint_tangent.find(common_edge)->second;
		}
		
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