#pragma once
#include "XObject.h"

class ThreeChordPatch;
class ThreeChordSegment;

class Patch {
private:
	MyViewer::MyTraits::Point central_point;
	std::vector<MyViewer::MyTraits::Point> twist_points;
	std::vector<MyViewer::MyTraits::Point> projected_twist_points;

	MyViewer::MyTraits::Point projectionToPlane(MyViewer::MyTraits::Point twist, OpenMesh::Vec3f face_normal);
	OpenMesh::Vec3f calculateBestFaceNormal();
	void drawLineSplit(std::vector<MyViewer::MyTraits::Point> points, Vec color);
	void drawWithNamesFace(std::vector<MyViewer::MyTraits::Point> points);
protected: 
	std::vector<XObject*> x_neighbours;
	std::vector<ThreeChordSegment*> bounding_curves;
	std::vector<ThreeChordPatch*> three_chord_patches;
	Vec color_control_net, color_patch;
	std::vector<BezierSurface*> bezier_patches;
	std::vector<MyViewer::MyTraits::Point> internal_points;

	MyViewer::MyTraits::Point findSharedE(XObject* x1, XObject* x2);
	std::vector<BezierSurface*> build4sidedPatch(SmoothingViewer::ConstructionMode mode);
	std::vector<BezierSurface*> centralSplit(SmoothingViewer::ConstructionMode mode);
	void attachCurveToX(ThreeChordSegment* curve, XObject* x);
	ThreeChordSegment* searchCommonCurve(XObject* x1, XObject* x2);
public:
	Patch(Vec color_control_net, Vec color_patch) {
		this->color_control_net = color_control_net;
		this->color_patch = color_patch;
	}

	void setNeighbour(XObject* x);
	virtual std::vector<BezierSurface*> setUpBezierSurface(SmoothingViewer::ConstructionMode mode) = 0;
	virtual std::vector<ThreeChordSegment*> setUpBoundingCurves() = 0;
	void drawControlNet(bool isBezier, bool isThreeChord) const;
	void drawOriginalFace();
	void drawBestFace();
	void drawWithNamesControl(bool isBezier, bool isThreeChord);
	void drawWithNamesOriginalFace();
	void drawWithNamesBestFace();
	void colorBezierSurfaces();
	void colorBezierSurfacesNames();
};