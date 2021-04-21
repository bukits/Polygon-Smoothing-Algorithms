#pragma once
#include "Patch.h"

class EPatch : public Patch {
private:
	SmoothingViewer::MyTraits::Point common_edge;
public:
	EPatch(Vec color_control, Vec color_patch) : Patch(color_control, color_patch) {}
	void setUpBezierSurface(SmoothingViewer::ConstructionMode mode, size_t resolution, std::vector<MyViewer::MyTraits::Point>&) override;
	std::vector<ThreeChordSegment*> setUpBoundingCurves() override;
};