#pragma once
#include "Patch.h"

class VPatch : public Patch {
private:
	MyViewer::MyTraits::Point vertex;
public:
	VPatch(Vec color_control, Vec color_patch) : Patch(color_control, color_patch) {}
	std::vector<ThreeChordSegment*> setUpBoundingCurves() override;
	void setUpBezierSurface(SmoothingViewer::ConstructionMode mode, size_t resolution, std::vector<MyViewer::MyTraits::Point>&) override;
};