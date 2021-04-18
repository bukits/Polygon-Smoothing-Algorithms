#include "ConstructionGenerator.h"
#include "ThreeChordSegment.h"
#include "VPatch.h"
#include "EPatch.h"
#include "FPatch.h"
#include "Line.h"

std::vector<XObject*> ConstructionGenerator::setUpXConstruction(SmoothingViewer::ConstructionMode construction_mode, double alpha) {
	patches.clear();
	central_points.clear();
	newEdges.clear();
	offset_lines.clear();
	xObjects_subdivison.clear();

	if (construction_mode == SmoothingViewer::ConstructionMode::SMOOTHING) {
		xObjects_subdivison = readMesh(SmoothingViewer::ConstructionMode::QUADRATIC, alpha);
		return readMesh(construction_mode, alpha);
	}
	else {
		return readMesh(construction_mode, alpha);
	}
}

std::vector<XObject*> ConstructionGenerator::readMesh(SmoothingViewer::ConstructionMode construction_mode, double alpha) {
	SmoothingViewer::MyTraits::Point barycenter;
	SmoothingViewer::PolyMesh::FaceVertexIter fv_it;
	SmoothingViewer::PolyMesh::VertexVertexIter vv_it;
	SmoothingViewer::PolyMesh::FaceIter f_it, f_end(mesh.faces_end());
	SmoothingViewer::PolyMesh::HalfedgeIter h_it, h_end(mesh.halfedges_end());

	std::vector<SmoothingViewer::MyTraits::Point> vertices_neighbour;

	int valence;

	std::vector<XObject*> xObjects;
	std::vector<SmoothingViewer::MyTraits::Point> vertexesPerFace;
	std::vector<SmoothingViewer::MyTraits::Point>::iterator it;

	for (f_it = mesh.faces_begin(); f_it != f_end; ++f_it) {
		vertexesPerFace.clear();
		barycenter[0] = barycenter[1] = barycenter[2] = valence = 0.0;
		for (fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it) {
			const auto& p = mesh.point(*fv_it);
			barycenter += p;
			++valence;
			vertexesPerFace.push_back(p);
		}
		barycenter /= valence;
		central_points.insert(barycenter);

		for (auto e : mesh.fe_range(f_it)) {
			auto midPoint = mesh.calc_edge_midpoint(e);
			std::pair<MyViewer::MyTraits::Point, MyViewer::MyTraits::Point> newEdge;
			newEdge.first = barycenter;
			newEdge.second = midPoint;
			newEdges.push_back(newEdge);
		}

		for (fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it) {
			vertices_neighbour.clear();
			for (vv_it = mesh.vv_iter(*fv_it); vv_it.is_valid(); vv_it++) {
				const auto& p_face = mesh.point(*vv_it);
				it = find(vertexesPerFace.begin(), vertexesPerFace.end(), p_face);
				if (it != vertexesPerFace.end()) {
					vertices_neighbour.push_back(p_face);
				}
			}
			ConstructionGenerator::FaceInfo faceInfo;
			faceInfo.p = mesh.point(*fv_it);
			faceInfo.barycenter = barycenter;
			const auto& p_n1 = vertices_neighbour.at(0);
			const auto& p_n2 = vertices_neighbour.at(1);
			faceInfo.originalE_1 = 0.5 * (faceInfo.p + p_n1);
			faceInfo.originalE_2 = 0.5 * (faceInfo.p + p_n2);
			faceInfo.newP = faceInfo.p;
			faceInfo.newE_1 = faceInfo.originalE_1;
			faceInfo.newE_2 = faceInfo.originalE_2;

			XObject* xObject;
			if (construction_mode == SmoothingViewer::ConstructionMode::QUADRATIC) xObject = useQuadraticBlending(faceInfo);
			if (construction_mode == SmoothingViewer::ConstructionMode::CUBIC) xObject = useCubicBlending(faceInfo, alpha);
			if (construction_mode == SmoothingViewer::ConstructionMode::SMOOTHING) xObject = useSmoothing(faceInfo);

			xObjects.push_back(xObject);
		}
	}
	return xObjects;
}

XObject* ConstructionGenerator::createX(ConstructionGenerator::XElements elements, ConstructionGenerator::FaceInfo faceInfo) {
	XObject* xObject = new XObject;
	xObject->u = elements.u;
	xObject->v = elements.v;
	xObject->x = elements.x;
	xObject->twist = elements.twist;
	xObject->u_opp = elements.u_opp;
	xObject->v_opp = elements.v_opp;
	xObject->twist_opp = elements.twist_opp;

	xObject->ePoint_tangent.insert({ elements.originalE_1, elements.u });
	xObject->ePoint_tangent.insert({ elements.originalE_2, elements.v });

	xObject->ePoint_tangentOpposite.insert({ elements.originalE_1, elements.v_opp });
	xObject->ePoint_tangentOpposite.insert({ elements.originalE_2, elements.u_opp });

	xObject->ePoint_twist_i.insert({ elements.originalE_1, elements.twist_1 });
	xObject->ePoint_twist_i.insert({ elements.originalE_2, elements.twist_2 });

	xObject->connected_face_bary_center = faceInfo.barycenter;
	xObject->connected_vertex = faceInfo.p;

	xObject->connected_e_points.push_back(elements.originalE_1);
	xObject->connected_e_points.push_back(elements.originalE_2);

	return xObject;
}

XObject* ConstructionGenerator::useQuadraticBlending(ConstructionGenerator::FaceInfo faceInfo) {
	auto h_1 = 0.5 * (faceInfo.newE_1 + faceInfo.newP);
	auto h_2 = 0.5 * (faceInfo.newE_2 + faceInfo.newP);
	auto q_1 = 0.5 * (faceInfo.barycenter + faceInfo.newE_1);
	auto q_2 = 0.5 * (faceInfo.barycenter + faceInfo.newE_2);

	auto x = 0.25 * (faceInfo.barycenter + faceInfo.newE_1 + faceInfo.newE_2 + faceInfo.newP);
	auto u = 0.5 * (h_1 + x);
	auto v = 0.5 * (h_2 + x);
	auto twist = 0.25 * (x + h_1 + faceInfo.newP + h_2);

	auto u_opp = 0.5 * (q_2 + x);
	auto v_opp = 0.5 * (q_1 + x);
	auto twist_opp = 0.25 * (faceInfo.barycenter + q_1 + q_2 + x);
	auto twist_1 = 0.25 * (q_1 + faceInfo.newE_1 + x + h_1);
	auto twist_2 = 0.25 * (q_2 + faceInfo.newE_2 + x + h_2);

	ConstructionGenerator::XElements elements;
	elements.x = x; elements.u = u; elements.v = v; elements.twist = twist;
	elements.twist_opp = twist_opp; elements.u_opp = u_opp, elements.v_opp = v_opp;
	elements.h_1 = h_1; elements.h_2 = h_2; elements.originalE_1 = faceInfo.originalE_1; elements.originalE_2 = faceInfo.originalE_2;
	elements.twist_1 = twist_1; elements.twist_2 = twist_2;

	return createX(elements, faceInfo);
}

XObject* ConstructionGenerator::useCubicBlending(FaceInfo faceInfo, float alpha) {
	auto dir_center1 = faceInfo.barycenter - faceInfo.newE_1;
	auto dir_center2 = faceInfo.barycenter - faceInfo.newE_2;

	auto dir_n1 = faceInfo.newE_1 - faceInfo.newP;
	auto dir_n2 = faceInfo.newE_2 - faceInfo.newP;

	auto edge1_normal = dir_center1.cross(dir_n1);
	auto edge2_normal = dir_center2.cross(dir_n2);
	
	auto translate_dir1 = -edge1_normal.cross(dir_n1);
	auto translate_dir2 = -edge2_normal.cross(dir_n2);

	auto p_1 = faceInfo.newE_1 + translate_dir1.normalize() * alpha;
	auto p_2 = faceInfo.newE_2 + translate_dir2.normalize() * alpha;

	Line* line1 = new Line(dir_n1, p_1);
	Line* line2 = new Line(dir_n2, p_2);

	Line* edge1 = new Line(dir_n1, faceInfo.newE_1);
	Line* edge2 = new Line(dir_n2, faceInfo.newE_2);

	Line* center_edge1 = new Line(dir_center1, faceInfo.newE_1);
	Line* center_edge2 = new Line(dir_center2, faceInfo.newE_2);

	auto x = line1->intersect(line2);

	auto h_1 = edge1->intersect(line2);
	auto h_2 = edge2->intersect(line1);
	auto q_1 = line1->intersect(center_edge1);
	auto q_2 = line2->intersect(center_edge2);

	auto u = (h_1 + x) * 0.5;
	auto v = (h_2 + x) * 0.5;
	auto twist = x + (u - x) + (v - x);

	auto u_opp = 0.5 * (q_2 + x);
	auto v_opp = 0.5 * (q_1 + x);
	auto twist_opp = x + (u_opp - x) + (v_opp - x);
	auto twist_1 = x + (u - x) + (v_opp - x);
	auto twist_2 = x + (u_opp - x) + (v - x);

	line1->setStart(q_1);
	line1->setEnd(x);
	offset_lines.push_back(line1);

	line2->setStart(q_2);
	line2->setEnd(x);
	offset_lines.push_back(line2);

	ConstructionGenerator::XElements elements;
	elements.x = x; elements.u = u; elements.v = v; elements.twist = twist;
	elements.twist_opp = twist_opp; elements.u_opp = u_opp, elements.v_opp = v_opp;
	elements.h_1 = h_1; elements.h_2 = h_2; elements.originalE_1 = faceInfo.originalE_1; elements.originalE_2 = faceInfo.originalE_2;
	elements.twist_1 = twist_1; elements.twist_2 = twist_2;

	return createX(elements, faceInfo);
}

XObject* ConstructionGenerator::useSmoothing(FaceInfo faceInfo) {
	std::vector<XObject*> e1_Objects;
	std::vector<XObject*> e2_Objects;
	std::vector<XObject*> p_Objects;
	for (auto xObj : xObjects_subdivison) {
		for (const auto& e_i : xObj->connected_e_points) {
			if (faceInfo.originalE_1 == e_i) e1_Objects.push_back(xObj);
			if (faceInfo.originalE_2 == e_i) e2_Objects.push_back(xObj);
		}
		if (xObj->connected_vertex == faceInfo.p) p_Objects.push_back(xObj);
	}

	SmoothingViewer::MyTraits::Point p_new, e1_new, e2_new;
	p_new[0] = p_new[1] = p_new[2] = 0.0;
	e1_new[0] = e1_new[1] = e1_new[2] = 0.0;
	e2_new[0] = e2_new[1] = e2_new[2] = 0.0;

	for (auto xObj : e1_Objects)
		e1_new += xObj->x;
	e1_new /= e1_Objects.size();

	for (auto xObj : e2_Objects)
		e2_new += xObj->x;
	e2_new /= e2_Objects.size();

	for (auto xObj : p_Objects)
		p_new += xObj->x;
	p_new /= p_Objects.size();

	ConstructionGenerator::FaceInfo faceInfoSubdivision;
	faceInfoSubdivision.p = faceInfo.p;
	faceInfoSubdivision.newP = p_new;
	faceInfoSubdivision.barycenter = faceInfo.barycenter;
	faceInfoSubdivision.originalE_1 = faceInfo.originalE_1;
	faceInfoSubdivision.originalE_2 = faceInfo.originalE_2;
	faceInfoSubdivision.newE_1 = e1_new;
	faceInfoSubdivision.newE_2 = e2_new;

	auto xObject = useQuadraticBlending(faceInfoSubdivision);
	return xObject;
}

std::vector<Patch*> ConstructionGenerator::generateVPatches(const std::vector<XObject*>& xObjects) {
	SmoothingViewer::PolyMesh::VertexIter v_it, v_end(mesh.vertices_end());
	SmoothingViewer::PolyMesh::VertexFaceIter vf_it;
	SmoothingViewer::PolyMesh::FaceVertexIter fv_it;
	SmoothingViewer::MyTraits::Point barycenter;
	int valence;
	std::vector<Patch*> v_patches;
	std::vector<XObject*> neighbours;
	for (v_it = mesh.vertices_begin(); v_it != v_end; ++v_it) {
		neighbours.clear();
		const auto& vertex = mesh.point(*v_it);
		for (vf_it = mesh.vf_iter(*v_it); vf_it.is_valid(); ++vf_it) {
			barycenter[0] = barycenter[1] = barycenter[2] = valence = 0.0;
			for (fv_it = mesh.fv_iter(*vf_it); fv_it.is_valid(); ++fv_it) {
				const auto& p = mesh.point(*fv_it);
				barycenter += p;
				++valence;
			}
			barycenter /= valence;
			for (auto x : xObjects) {
				if (x->connected_face_bary_center == barycenter && x->connected_vertex == vertex)
					neighbours.push_back(x);
			}
		}
		if (neighbours.size() >= 3) {
			Patch* patch = new VPatch(vertex_control_color, vertex_patch_color);
			for (auto x_neighbour : neighbours)
				patch->setNeighbour(x_neighbour);
			v_patches.push_back(patch);
			patches.push_back(patch);
		}
	}
	return v_patches;
}

std::vector<Patch*> ConstructionGenerator::generateEPatches(const std::vector<XObject*>& xObjects) {
	SmoothingViewer::PolyMesh::EdgeIter e_it, e_end(mesh.edges_end());
	std::vector<Patch*> e_patches;
	std::vector<XObject*> neighbours;
	std::map<SmoothingViewer::MyTraits::Point, XObject*> point_xObject;
	for (e_it = mesh.edges_begin(); e_it != e_end; ++e_it) {
		neighbours.clear();
		point_xObject.clear();
		auto e_half_point = mesh.calc_edge_midpoint(e_it);
		for (auto x : xObjects) {
			if (e_half_point == x->connected_e_points.at(0) || e_half_point == x->connected_e_points.at(1)) {
				neighbours.push_back(x);
				point_xObject.insert({ x->x, x });
			}
		}
		if (neighbours.size() == 4) {
			Patch* patch = new EPatch(edge_control_color, edge_patch_color);
			auto x1 = neighbours.at(0);
			auto x2 = neighbours.at(1);
			patch->setNeighbour(x1);
			point_xObject.erase(x1->x);
			patch->setNeighbour(x2);
			point_xObject.erase(x2->x);
			for (const auto& e_i : x2->connected_e_points) {
				for (auto x_other : point_xObject) {
					auto v = x_other.second->connected_vertex;
					auto ePoints = x_other.second->connected_e_points;
					const auto& e_1 = ePoints.at(0);
					const auto& e_2 = ePoints.at(1);
					if ((e_i == e_1 || e_i == e_2) && v == x2->connected_vertex) {
						patch->setNeighbour(x_other.second);
						point_xObject.erase(x_other.first);
						break;
					}
				}
			}
			for (auto x_other : point_xObject) patch->setNeighbour(x_other.second);
			patches.push_back(patch);
			e_patches.push_back(patch);
		}
	}
	return e_patches;
}

std::vector<Patch*> ConstructionGenerator::generateFPatches(const std::vector<XObject*>& xObjects) {
	Patch* patch;
	std::vector<Patch*> f_patches;
	for (const auto& central_point : central_points) {
		patch = new FPatch(face_control_color, face_patch_color);
		for (auto x_obj : xObjects) {
			if (central_point == x_obj->connected_face_bary_center) {
				patch->setNeighbour(x_obj);
			}
		}
		patches.push_back(patch);
		f_patches.push_back(patch);
	}
	return f_patches;
}

std::vector<BezierSurface*> ConstructionGenerator::generateBezierSurfacesFromPatces(SmoothingViewer::ConstructionMode mode) {
	std::vector<BezierSurface*> surfaces;
	for (auto patch : patches)
		for (auto surface : patch->setUpBezierSurface(mode))
			surfaces.push_back(surface);
	return surfaces;
}

std::vector<BezierCurve*> ConstructionGenerator::generateBoundingCurves(SmoothingViewer::ConstructionMode mode) {
	std::vector<BezierCurve*> curves;
	std::vector<SmoothingViewer::MyTraits::Point>::iterator it;
	std::vector<SmoothingViewer::MyTraits::Point> half_points;
	for (auto patch : patches) {
		for (auto curve_segment : patch->setUpBoundingCurves()) {
			auto half = curve_segment->getHalfPoint();
			it = find(half_points.begin(), half_points.end(), half);
			if (it == half_points.end()) {
				half_points.push_back(half);
				curves.push_back(curve_segment->generateCurve(mode));
			}
		}
	}
	return curves;
}

std::set<SmoothingViewer::PolyMesh::Point> ConstructionGenerator::getCentralPoints() {
	return central_points;
}

std::vector<std::pair<MyViewer::MyTraits::Point, MyViewer::MyTraits::Point>> ConstructionGenerator::getEdges() {
	return newEdges;
}

std::vector<Patch*> ConstructionGenerator::getPatches() {
	return patches;
}

std::vector<Line*> ConstructionGenerator::getOffsetLines() {
	return offset_lines;
}