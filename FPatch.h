#pragma once
#include "Patch.h"

class FPatch : public Patch {
private:
	MyViewer::MyTraits::Point central_point;
public:
	FPatch(Vec color_control, Vec color_patch) : Patch(color_control, color_patch) {}
	void setUpBezierSurface(SmoothingViewer::ConstructionMode mode) override;
	void generateBezierSurface(size_t resolution, std::vector<MyViewer::MyTraits::Point>&) override;
	std::vector<ThreeChordSegment*> setUpBoundingCurves() override;
};