#pragma once

#include "MyViewer.h"

class Line {
private:
	OpenMesh::Vec3d direction;
	MyViewer::MyTraits::Point point;

	MyViewer::MyTraits::Point p_start, p_end;
public:
	Line(OpenMesh::Vec3d direction,
	MyViewer::MyTraits::Point point) {
		this->direction = direction;
		this->point = point;
	}

	MyViewer::MyTraits::Point intersect(Line* line);
	bool isIntersected(Line* line);
	MyViewer::MyTraits::Point getStart();
	MyViewer::MyTraits::Point getEnd();
	void setStart(MyViewer::MyTraits::Point p);
	void setEnd(MyViewer::MyTraits::Point p);
};