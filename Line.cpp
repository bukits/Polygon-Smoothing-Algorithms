#include "Line.h"

MyViewer::MyTraits::Point Line::intersect(Line* line) {
    const auto& v2 = line->direction;
    const auto& p2 = line->point;
    MyViewer::MyTraits::Point result;

    auto v3 = (p2 - point).cross(v2);

    auto v4 = direction.cross(v2);

    float dot = v4.dot(v4);

    // if both V1 and V2 are same direction, return NaN point
    if (dot == 0)
        return result;

    float alpha = v3.dot(v4) / dot;

    // find intersect point
    result = point + (alpha * direction);
    return result;
}

bool Line::isIntersected(Line* line) {
    auto v = this->direction.cross(line->direction);
    if (v[0] == 0 && v[1] == 0 && v[2] == 0)
        return false;
    else
        return true;
}

MyViewer::MyTraits::Point Line::getStart() {
    return this->p_start;
}

MyViewer::MyTraits::Point Line::getEnd() {
    return this->p_end;
}

void Line::setStart(MyViewer::MyTraits::Point p) {
    this->p_start = p;
}

void Line::setEnd(MyViewer::MyTraits::Point p) {
    this->p_end = p;
}