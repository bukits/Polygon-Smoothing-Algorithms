#include "SmoothingViewer.h"

class ThreeChordSegment;

struct XObject {
	MyViewer::MyTraits::Point x, u, v, u_opp, v_opp, twist, twist_opp;
	std::map<MyViewer::MyTraits::Point, MyViewer::MyTraits::Point> ePoint_tangentOpposite;
	std::map<MyViewer::MyTraits::Point, MyViewer::MyTraits::Point> ePoint_tangent;
	std::map<MyViewer::MyTraits::Point, MyViewer::MyTraits::Point> ePoint_twist_i;

	MyViewer::MyTraits::Point connected_face_bary_center;
	MyViewer::MyTraits::Point connected_vertex;
	std::map<MyViewer::MyTraits::Point, ThreeChordSegment*> connected_curves;
	std::vector<MyViewer::MyTraits::Point> connected_e_points;
};