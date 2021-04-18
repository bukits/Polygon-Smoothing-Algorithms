#pragma once
#include "Patch.h"

class VPatch : public Patch {
public:
	VPatch(Vec color_control, Vec color_patch) : Patch(color_control, color_patch) {}
	std::vector<ThreeChordSegment*> setUpBoundingCurves() override;
	std::vector<BezierSurface*> setUpBezierSurface(SmoothingViewer::ConstructionMode mode) override;
};