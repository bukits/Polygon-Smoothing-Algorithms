#include <fstream>
#include <iostream>
#include "MyViewer.h"

class STLProcessor {
private: 
	std::vector<MyViewer::MyTraits::Point> vertices;
	const std::string stl_file = "smoothed-mesh.stl";

	void STLFacetOut(std::ofstream&, std::vector<MyViewer::MyTraits::Point>::const_iterator, MyViewer::Vector);
	void STLExport(std::ofstream&);
public:
	STLProcessor() {}
	void writeSTL(std::vector<MyViewer::MyTraits::Point>&);
	bool readSTL(MyViewer::PolyMesh&);
};