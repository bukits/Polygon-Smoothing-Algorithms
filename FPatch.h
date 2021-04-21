#pragma once
#include "Patch.h"

class FPatch : public Patch {
public:
	FPatch(Vec color_control, Vec color_patch) : Patch(color_control, color_patch) {}
	void setUpBezierSurface(SmoothingViewer::ConstructionMode mode, size_t resolution, std::vector<MyViewer::MyTraits::Point>&) override;
	std::vector<ThreeChordSegment*> setUpBoundingCurves() override;
};