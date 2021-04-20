#include "ThreeChordSegment.h"

BezierCurve* ThreeChordSegment::buildCurve(SmoothingViewer::ConstructionMode curve_mode) {
    BezierCurve* bezier_curve = new BezierCurve;

    SmoothingViewer::MyTraits::Point 
        M_0 = three_chord_cps[0], M_1 = three_chord_cps[1],
        M_2 = three_chord_cps[2], M_3 = three_chord_cps[3];
    SmoothingViewer::MyTraits::Point B_0, B_1, B_2, B_3;

    B_0 = M_0;
    bezier_curve->addControlPoint(B_0);
    bezier_cps.push_back(B_0);
   
    left_segment[0] = M_0;
    left_segment[1] = (M_0 + M_1) * 0.5;
    left_segment[3] = half_point;

    right_segment[0] = M_3;
    right_segment[1] = (M_2 + M_3) * 0.5;
    right_segment[3] = half_point;
    if (curve_mode == SmoothingViewer::ConstructionMode::QUADRATIC) {
       
        left_segment[2] = (M_1 + half_point) * 0.5;
        right_segment[2] = (M_2 + half_point) * 0.5;

        B_2 = M_3;
        B_1 = M_0 + translatePointBy(2, M_1, M_0);
        bezier_curve->addControlPoint(B_1);
        bezier_curve->addControlPoint(B_2);
        return bezier_curve;
    }
    else if (curve_mode == SmoothingViewer::ConstructionMode::CUBIC) {
        B_3 = M_3;
        B_1 = M_0 + translatePointBy(4 / 3.0f, M_1, M_0);
        B_2 = M_3 + translatePointBy(4 / 3.0f, M_2, M_3);

        auto B_01 = 0.5 * (B_0 + B_1);
        auto B_12 = 0.5 * (B_1 + B_2);
        auto B_012 = 0.5 * (B_01 + B_12);
        left_segment[2] = left_segment[3] + translatePointBy(0.75, B_012, left_segment[3]);

        auto B_23 = 0.5 * (B_2 + B_3);
        auto B_123 = 0.5 * (B_12 + B_23);
        right_segment[2] = right_segment[3] + translatePointBy(0.75, B_123, right_segment[3]);

        bezier_curve->addControlPoint(B_1);
        bezier_cps.push_back(B_1);
        bezier_curve->addControlPoint(B_2);
        bezier_cps.push_back(B_2);
        bezier_curve->addControlPoint(B_3);
        bezier_cps.push_back(B_3);
        return bezier_curve;
    }
 }

MyViewer::MyTraits::Point ThreeChordSegment::translatePointBy(float rate, MyViewer::MyTraits::Point p1, MyViewer::MyTraits::Point p2) {
    auto dir = p1 - p2;
    return rate * dir.length() * dir.normalize();
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

MyViewer::MyTraits::Point ThreeChordSegment::getBezierCps(int index) {
    return bezier_cps.at(index);
}