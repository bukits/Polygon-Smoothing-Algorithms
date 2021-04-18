#pragma once
#include "Patch.h"

class FPatch : public Patch {
public:
	FPatch(Vec color_control, Vec color_patch) : Patch(color_control, color_patch) {}
	std::vector<BezierSurface*> setUpBezierSurface(SmoothingViewer::ConstructionMode mode) override;
	std::vector<ThreeChordSegment*> setUpBoundingCurves() override;
};