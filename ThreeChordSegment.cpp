#include "ThreeChordSegment.h"

BezierCurve* ThreeChordSegment::generateCurve(SmoothingViewer::ConstructionMode curve_mode) {
    BezierCurve* bezier_curve = new BezierCurve;

    SmoothingViewer::MyTraits::Point 
        M_0 = three_chord_cps[0], M_1 = three_chord_cps[1],
        M_2 = three_chord_cps[2], M_3 = three_chord_cps[3];
    SmoothingViewer::MyTraits::Point B_0, B_1, B_2, B_3;

    B_0 = M_0;
    bezier_curve->addControlPoint(B_0);
    auto direction = M_1 - M_0;
    auto len = direction.length();
    if (len != 0.0)
        direction /= len;
    if (curve_mode == SmoothingViewer::ConstructionMode::QUADRATIC) {
        B_2 = M_3;
        B_1 = direction * 2.0 * len + M_0;
        bezier_curve->addControlPoint(B_1);
        bezier_curve->addControlPoint(B_2);
        return bezier_curve;
    }
    else if (curve_mode == SmoothingViewer::ConstructionMode::CUBIC) {
        B_3 = M_3;
        auto direction_other = M_2 - M_3;
        auto len_other = direction_other.length();

        if (len_other != 0.0)
            direction_other /= len_other;
        B_1 = direction * len * (4 / 3) + M_0;
        B_2 = direction_other * len_other * (4 / 3) + M_3;
        bezier_curve->addControlPoint(B_1);
        bezier_curve->addControlPoint(B_2);
        bezier_curve->addControlPoint(B_3);
        return bezier_curve;
    }
 }

MyViewer::MyTraits::Point ThreeChordSegment::getThreeChordCp(int index) {
    return this->three_chord_cps[index];
}

MyViewer::MyTraits::Point ThreeChordSegment::getLeftSegmentPoint(int at) {
    return left_segment[at];
}

MyViewer::MyTraits::Point ThreeChordSegment::getRightSegmentPoint(int at) {
    return right_segment[at];
}

MyViewer::MyTraits::Point ThreeChordSegment::getHalfPoint() {
    return half_point;
}