#include "STLProcessor.h"
#include <OpenMesh/Core/IO/MeshIO.hh>

void STLProcessor::STLExport(std::ofstream& stlout) {
	stlout << "solid STLExport" << std::endl;
	for (auto q = vertices.begin(); q < vertices.end(); q += 3) {
		auto normal = (q[1] - q[0]).cross(q[2] - q[1]).normalize();
		STLFacetOut(stlout, q, normal);
	}
	stlout << "endsolid STLExport" << std::endl;
}

void STLProcessor::STLFacetOut(std::ofstream& stlout, std::vector<MyViewer::MyTraits::Point>::const_iterator vv, MyViewer::Vector normal) {
	stlout << "facet normal " << normal[0] << " " << normal[1] << " " << normal[2] << std::endl;
	stlout << "   outer loop" << std::endl;
	stlout << "      vertex " << vv[0][0] << " " << vv[0][1] << " " << vv[0][2] << std::endl;
	stlout << "      vertex " << vv[1][0] << " " << vv[1][1] << " " << vv[1][2] << std::endl;
	stlout << "      vertex " << vv[2][0] << " " << vv[2][1] << " " << vv[2][2] << std::endl;
	stlout << "   endloop" << std::endl;
	stlout << "endfacet" << std::endl;
}

void STLProcessor::writeSTL(std::vector<MyViewer::MyTraits::Point>& bezier_vertices) {
	for (const auto& vertice : bezier_vertices) vertices.push_back(vertice);
	std::ofstream stlout(stl_file, std::ofstream::trunc);
	STLExport(stlout);
	stlout.close();
}

bool STLProcessor::readSTL(MyViewer::PolyMesh& mesh) {
	if (!OpenMesh::IO::read_mesh(mesh, stl_file) || mesh.n_vertices() == 0)
		return false;
	return true;
}