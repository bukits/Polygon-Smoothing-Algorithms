#include "SmoothingViewer.h"

class Line;

class ConstructionGenerator {
public:
	ConstructionGenerator(SmoothingViewer::PolyMesh mesh) {
		this->mesh = mesh;
		this->vertex_control_color = Vec(1.0, 0.0, 0.0);
		this->face_control_color = Vec(0.7, 1.0, 0.0);
		this->edge_control_color = Vec(0.694, 0.039, 0.557);

		vertex_patch_color = Vec(0.737, 0.102, 0.086);
		face_patch_color = Vec(0.059, 0.247, 0.635);
		edge_patch_color = Vec(0.039, 0.694, 0.506);
	}

	std::vector<XObject*> setUpXConstruction(SmoothingViewer::ConstructionMode construction_mode, double alpha);

	void generateBezierSurfacesFromPatces(SmoothingViewer::ConstructionMode mode, size_t resolution, SmoothingViewer::PolyMesh& mesh);

	std::vector<Patch*> generateVPatches(const std::vector<XObject*>& xObjects);
	std::vector<Patch*> generateEPatches(const std::vector<XObject*>& xObjects);
	std::vector<Patch*> generateFPatches(const std::vector<XObject*>& xObjects);

	std::vector<BezierCurve*> generateBoundingCurves(SmoothingViewer::ConstructionMode mode);

	std::set<MyViewer::MyTraits::Point> getCentralPoints();
	std::vector<std::pair<MyViewer::MyTraits::Point, MyViewer::MyTraits::Point>> getEdges();
	std::vector<Patch*> getPatches();
	std::vector<Line*> getOffsetLines();
private:
	SmoothingViewer::PolyMesh mesh;
	std::vector<Patch*> patches;
	std::set<MyViewer::MyTraits::Point> central_points;
	std::vector<std::pair<MyViewer::MyTraits::Point, MyViewer::MyTraits::Point>> newEdges;
	std::vector<Line*> offset_lines;
	std::vector<XObject*> xObjects_subdivison;

	Vec vertex_control_color;
	Vec face_control_color;
	Vec edge_control_color;

	Vec vertex_patch_color;
	Vec face_patch_color;
	Vec edge_patch_color;

	struct FaceInfo {
		MyViewer::MyTraits::Point barycenter, p, originalE_1, originalE_2, newE_1, newE_2, newP;
	};

	struct XElements {
		MyViewer::MyTraits::Point x, u, v, twist, twist_opp, u_opp, v_opp, h_1, h_2, originalE_1, originalE_2, twist_1, twist_2;
	};

	XObject* createX(XElements elements, FaceInfo info);

	std::vector<XObject*> readMesh(SmoothingViewer::ConstructionMode construction_mode, double alpha);

	XObject* useQuadraticBlending(FaceInfo faceInfo);
	XObject* useCubicBlending(FaceInfo faceInfo, float alpha);
	XObject* useSmoothing(FaceInfo faceInfo);
};
