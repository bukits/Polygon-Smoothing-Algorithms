#pragma once
#include "Patch.h"

class EPatch : public Patch {
private:
	SmoothingViewer::MyTraits::Point common_edge;
public:
	EPatch(Vec color_control, Vec color_patch) : Patch(color_control, color_patch) {}
	std::vector<BezierSurface*> setUpBezierSurface(SmoothingViewer::ConstructionMode mode) override;
	std::vector<ThreeChordSegment*> setUpBoundingCurves() override;
};