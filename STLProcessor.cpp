#include "STLProcessor.h"
#include <OpenMesh/Core/IO/MeshIO.hh>

//void STLProcessor::STLExport(std::ofstream& stlout) {
//	std::string header_info = "output";
//	char head[80];
//	std::strncpy(head, header_info.c_str(), sizeof(head) - 1);
//	unsigned short attribute = 0;
//	unsigned long nTriLong = vertices.size();
//	stlout.write(head, sizeof head);
//	stlout.write((char*)&nTriLong, sizeof nTriLong);
//	for (auto q = vertices.begin(); q < vertices.end(); q += 3) {
//		auto normal = (q[1] - q[0]).cross(q[2] - q[1]).normalize();
//		STLFacetOut(stlout, q, normal);
//		stlout.write((char*)&attribute, sizeof attribute);
//	}
//}
//
//void STLProcessor::STLFacetOut(std::ofstream& stlout, std::vector<MyViewer::MyTraits::Point>::const_iterator vv, MyViewer::Vector normal) {
//	float normal0 = normal[0];
//	float normal1 = normal[1];
//	float normal2 = normal[2];
//	stlout.write((char*)&normal0, sizeof normal0);
//	stlout.write((char*)&normal1, sizeof normal1);
//	stlout.write((char*)&normal2, sizeof normal2);
//
//	float vertex1_x = vv[0][0];
//	float vertex1_y = vv[0][1];
//	float vertex1_z = vv[0][2];
//	stlout.write((char*)&vertex1_x, sizeof vertex1_x);
//	stlout.write((char*)&vertex1_y, sizeof vertex1_y);
//	stlout.write((char*)&vertex1_z, sizeof vertex1_z);
//
//	float vertex2_x = vv[1][0];
//	float vertex2_y = vv[1][1];
//	float vertex2_z = vv[1][2];
//	stlout.write((char*)&vertex2_x, sizeof vertex2_x);
//	stlout.write((char*)&vertex2_y, sizeof vertex2_x);
//	stlout.write((char*)&vertex2_z, sizeof vertex2_z);
//
//	float vertex3_x = vv[2][0];
//	float vertex3_y = vv[2][1];
//	float vertex3_z = vv[2][2];
//	stlout.write((char*)&vertex3_x, sizeof vertex3_x);
//	stlout.write((char*)&vertex3_y, sizeof vertex3_y);
//	stlout.write((char*)&vertex3_z, sizeof vertex3_z);
//}

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
	vertices = bezier_vertices;
	std::ofstream stlout;
	stlout.open((stl_file).c_str(), std::ios::out | std::ios::binary);
	STLExport(stlout);
	stlout.close();
}

bool STLProcessor::readSTL(MyViewer::PolyMesh& mesh) {
	if (!OpenMesh::IO::read_mesh(mesh, stl_file) || mesh.n_vertices() == 0)
		return false;
	return true;
}