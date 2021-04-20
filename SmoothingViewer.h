#pragma once

#include "MyViewer.h"
#include "BezierSurface.h"
#include "BezierCurve.h"

class ConstructionGenerator;
class XObject;
class Patch;

class SmoothingViewer : public MyViewer {
public:

	SmoothingViewer(QWidget* parent);

	enum class ConstructionMode { QUADRATIC, CUBIC, SMOOTHING };
	static const size_t resolution_surface = 30;
	static const size_t resolution_curve = 30;

	void showBaryC();
	void showEdges();
	void showXCps();
	void showXObj();
	void showBoundingNet();
	void showVPatches();
	void showEPatches();
	void showFPatches();
	void showBezierStrucure();
	void showThreeChordStructure();
	void showOriginalFace();
	void showBestFace();
	void showControlWireFrame();
	void showOffsetLines();
	void showColoredPatches();
	void showSmoothedMesh();

	bool getBaryCState();
	bool getEdgesState();
	bool getXCpsState();
	bool getXObjState();
	bool getBoundingNetState();
	bool getVPatchesState();
	bool getEPatchesState();
	bool getFPatchesState();
	bool getBezierStructureState();
	bool getThreeChorStructureState();
	bool getOriginalFaceState();
	bool getBestFaceState();
	bool getControlWireFrameState();
	bool getOffsetLinesState();
	bool getColoredPatchesState();

	bool openMesh(const std::string& filename, bool update_view = true) override;
	void setUpConstruction(ConstructionMode construction_mode, ConstructionMode curve_mode, double alpha = 0.1);

protected:
	void draw() override;
	void drawWithNames() override;

private:
	ConstructionGenerator* used_construction;
	std::vector<XObject*> xObjects;
	std::vector<std::pair<MyTraits::Point, MyTraits::Point>> new_edges;

	std::vector<Patch*> patches;
	std::vector<BezierCurve*> curves;

	PolyMesh control_mesh;

	std::vector<Patch*> v_patches;
	std::vector<Patch*> e_patches;
	std::vector<Patch*> f_patches;

	std::vector<MyTraits::Point> curve_points;

	bool show_barycenter_points,
		show_xconstruction_points,
		show_edges,
		show_x,
		show_bounding_net,
		show_v_patches_controlnet,
		show_e_patches_controlnet,
		show_f_patches_controlnet,
		show_bezier_structure,
		show_three_chord_structure,
		show_original_face,
		show_best_face,
		show_control_wireframe,
		show_offset_lines,
		show_smoothed_mesh;

	void drawBoundingNet() const;
	void generateBoundigNet(ConstructionMode curve_mode, size_t resolution = resolution_curve);
	void generateSmoothedMesh(ConstructionMode curve_mode, size_t resolution = resolution_surface);
	void clearViewer();
};