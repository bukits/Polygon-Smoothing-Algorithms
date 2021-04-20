#include "SmoothingViewer.h"
#include "ConstructionGenerator.h"
#include "ThreeChordSegment.h"
#include "Patch.h"
#include "Line.h"

SmoothingViewer::SmoothingViewer(QWidget* parent) : MyViewer(parent),
        show_barycenter_points(false),
        show_xconstruction_points(false),
        show_edges(false),
        show_x(false),
        show_bounding_net(false),
        show_v_patches_controlnet(false),
        show_e_patches_controlnet(false),
        show_f_patches_controlnet(false),
        show_bezier_structure(false),
        show_three_chord_structure(false),
        show_original_face(false),
        show_best_face(false),
        show_control_wireframe(false),
        show_offset_lines(false),
        show_smoothed_mesh(false)
{}

void::SmoothingViewer::setUpConstruction(ConstructionMode construction_mode, ConstructionMode curve_mode, double alpha) {
    clearViewer();
    xObjects = used_construction->setUpXConstruction(construction_mode, alpha);

    auto vp_container = used_construction->generateVPatches(xObjects);
    auto fp_container = used_construction->generateFPatches(xObjects);
    auto ep_container = used_construction->generateEPatches(xObjects);

    patches = used_construction->getPatches();
    new_edges = used_construction->getEdges();

    copy(vp_container.begin(), vp_container.end(), back_inserter(v_patches));
    copy(ep_container.begin(), ep_container.end(), back_inserter(e_patches));
    copy(fp_container.begin(), fp_container.end(), back_inserter(f_patches));

    generateBoundigNet(curve_mode);
    generateSmoothedMesh(curve_mode);
}

void SmoothingViewer::clearViewer() {
    v_patches.clear();
    e_patches.clear();
    f_patches.clear();
    patches.clear();
    new_edges.clear();
    curves.clear();
    curve_points.clear();
    xObjects.clear();
}

bool SmoothingViewer::openMesh(const std::string& filename, bool update_view) {
    clearViewer();
    bool success = __super::openMesh(filename, update_view);
    control_mesh = mesh;
    this->show_colored_patches = false;
    used_construction = new ConstructionGenerator(mesh);
    return success;
}

void SmoothingViewer::draw() {
    __super::draw();

    if (show_barycenter_points) {
        glColor3d(1.0, 0.0, 1.0);
        glPointSize(5.0);
        glDisable(GL_LIGHTING);
        glBegin(GL_POINTS);
        for (const auto& p : used_construction->getCentralPoints()) {
            Vec vec(p[0], p[1], p[2]);
            glVertex3dv(vec);
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }

    if (show_colored_patches) {
        glDisable(GL_LIGHTING);
        for (auto patch : patches) {
            patch->colorBezierSurfaces();
        }
        glEnable(GL_LIGHTING);
    }

    if (show_xconstruction_points) {
        glColor3d(1.0, 0.0, 1.0);
        glPointSize(5.0);
        glDisable(GL_LIGHTING);
        glBegin(GL_POINTS);
        for (const auto& xObj : xObjects) {
            MyTraits::Point const& u = xObj->u;
            Vec U(u[0], u[1], u[2]);
            glVertex3dv(U);
            MyTraits::Point const& v = xObj->v;
            Vec V(v[0], v[1], v[2]);
            glVertex3dv(V);
            MyTraits::Point const& x = xObj->x;
            Vec X(x[0], x[1], x[2]);
            glVertex3dv(X);
            MyTraits::Point const& twist = xObj->twist;
            Vec T(twist[0], twist[1], twist[2]);
            glVertex3dv(T);
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }

    if (show_edges) {
        glLineWidth(3.0);
        glColor3d(0.0, 0.0, 1.0);
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        for (auto edge : new_edges) {
            glVertex3dv(edge.first.data());
            glVertex3dv(edge.second.data());
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }

    if (show_x) {
        glLineWidth(3.0);
        glDisable(GL_LIGHTING);
        for (auto xObj : xObjects) {
            glColor3d(0.0, 1.0, 0.0);
            glBegin(GL_LINES);
            glVertex3dv(xObj->u.data());
            glVertex3dv(xObj->u_opp.data());
            glEnd();
            glColor3d(0.0, 1.0, 0.0);
            glBegin(GL_LINES);
            glVertex3dv(xObj->v.data());
            glVertex3dv(xObj->v_opp.data());
            glEnd();
        }
        glEnable(GL_LIGHTING);
    }

    if (show_bounding_net) {
        glDisable(GL_LIGHTING);
        drawBoundingNet();
        glEnable(GL_LIGHTING);
    }

    if (show_v_patches_controlnet) {
        glDisable(GL_LIGHTING);
        for (auto patch : v_patches)
            patch->drawControlNet(show_bezier_structure, show_three_chord_structure);
        glEnable(GL_LIGHTING);
    }

    if (show_e_patches_controlnet) {
        glDisable(GL_LIGHTING);
        for (auto patch : e_patches)
            patch->drawControlNet(show_bezier_structure, show_three_chord_structure);
        glEnable(GL_LIGHTING);
    }

    if (show_f_patches_controlnet) {
        glDisable(GL_LIGHTING);
        for (auto patch : f_patches)
            patch->drawControlNet(show_bezier_structure, show_three_chord_structure);
        glEnable(GL_LIGHTING);
    }

    if (show_original_face) {
        glDisable(GL_LIGHTING);
        for (auto patch : patches)
            patch->drawOriginalFace();
        glEnable(GL_LIGHTING);
    }

    if (show_best_face) {
        glDisable(GL_LIGHTING);
        for (auto patch : patches)
            patch->drawBestFace();
        glEnable(GL_LIGHTING);
    }

    if (show_control_wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3d(0.0, 0.0, 0.0);
        glDisable(GL_LIGHTING);
        for (auto f : control_mesh.faces()) {
            glBegin(GL_POLYGON);
            for (auto v : control_mesh.fv_range(f))
                glVertex3dv(control_mesh.point(v).data());
            glEnd();
        }
        glEnable(GL_LIGHTING);
    }

    if (show_offset_lines) {
        glLineWidth(3.0);
        glColor3d(1.0, 0.0, 1.0);
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        for (auto line : used_construction->getOffsetLines()) {
            auto p_1 = line->getStart();
            auto p_2 = line->getEnd();
            glVertex3dv(p_1.data());
            glVertex3dv(p_2.data());
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }
}

void SmoothingViewer::generateSmoothedMesh(ConstructionMode curve_mode, size_t resolution) {
    mesh.clean();
    auto generated_bezier_surfaces = used_construction->generateBezierSurfacesFromPatces(curve_mode);
    for (auto bezier : generated_bezier_surfaces) {
        bezier->generateSurface(resolution, mesh);
    }
    show_colored_patches = true;
    this->updateMesh();
    this->update();
}

void SmoothingViewer::drawWithNames() {
    __super::drawWithNames();
    switch (model_type) {
    case ModelType::MESH:
        if (show_barycenter_points) {
            std::set<MyTraits::Point>::iterator cp;
            size_t i = 0;
            for (cp = used_construction->getCentralPoints().begin(); cp != used_construction->getCentralPoints().end(); ++cp, i++) {
                MyTraits::Point const& p = *cp;
                Vec vec(p[0], p[1], p[2]);
                glPushName(i);
                glRasterPos3fv(vec);
                glPopName();
            }
        }

        if (show_colored_patches) {
            for (auto patch : patches) {
                patch->colorBezierSurfacesNames();
            }
        }
        if (show_xconstruction_points) {
            for (size_t i = 0, ie = xObjects.size(); i < ie; ++i) {
                MyTraits::Point const& u = xObjects[i]->u;
                Vec U(u[0], u[1], u[2]);
                glPushName(i * 3 + 0);
                glRasterPos3fv(U);
                glPopName();
                MyTraits::Point const& v = xObjects[i]->v;
                Vec V(v[0], v[1], v[2]);
                glPushName(i * 3 + 1);
                glRasterPos3fv(V);
                glPopName();
                MyTraits::Point const& x = xObjects[i]->x;
                Vec X(x[0], x[1], x[2]);
                glPushName(i * 3 + 2);
                glRasterPos3fv(X);
                glPopName();
                MyTraits::Point const& t = xObjects[i]->twist;
                Vec T(t[0], t[1], t[2]);
                glPushName(i * 3 + 2);
                glRasterPos3fv(T);
                glPopName();
            }
        }
        if (show_edges) {
            for (size_t i = 0, ie = new_edges.size(); i < ie; ++i) {
                MyTraits::Point const& bary_center = new_edges[i].first;
                Vec bary(bary_center[0], bary_center[1], bary_center[2]);
                glPushName(i * 2);
                glRasterPos3dv(bary);
                MyTraits::Point const& mid_point = new_edges[i].second;
                Vec mid(mid_point[0], mid_point[1], mid_point[2]);
                glPopName();
                glPushName(i * 2 + 1);
                glRasterPos3dv(mid);
                glPopName();
            }
        }
        if (show_x) {
            for (size_t i = 0, ie = xObjects.size(); i < ie; ++i) {
                MyTraits::Point const& u = xObjects[i]->u;
                Vec U(u[0], u[1], u[2]);
                glPushName(i * 4);
                glRasterPos3fv(U);
                glPopName();
                MyTraits::Point const& u_1 = xObjects[i]->u_opp;
                Vec U_1(u_1[0], u_1[1], u_1[2]);
                glPushName(i * 4 + 1);
                glRasterPos3fv(U_1);
                glPopName();
                MyTraits::Point const& v = xObjects[i]->v;
                Vec V(v[0], v[1], v[2]);
                glPushName(i * 4 + 2);
                glRasterPos3fv(V);
                glPopName();
                MyTraits::Point const& v_1 = xObjects[i]->v_opp;
                Vec V_1(v_1[0], v_1[1], v_1[2]);
                glPushName(i * 4 + 3);
                glRasterPos3fv(V_1);
                glPopName();

            }
        }
        if (show_bounding_net) {
            for (size_t i = 0, ie = curve_points.size(); i < ie; ++i) {
                MyTraits::Point const& p = curve_points[i];
                Vec P(p[0], p[1], p[2]);
                glPushName(i);
                glRasterPos3fv(P);
                glPopName();
            }
        }
        if (show_v_patches_controlnet) {
            for (auto patch : v_patches) 
                patch->drawWithNamesControl(show_bezier_structure, show_three_chord_structure);
        }
        if (show_e_patches_controlnet) {
            for (auto patch : e_patches)
                patch->drawWithNamesControl(show_bezier_structure, show_three_chord_structure);
        }
        if (show_f_patches_controlnet) {
            for (auto patch : f_patches)
                patch->drawWithNamesControl(show_bezier_structure, show_three_chord_structure);
        }

        if (show_original_face) {
            for (auto patch : patches)
                patch->drawWithNamesOriginalFace();
        }

        if (show_best_face) {
            for (auto patch : patches)
                patch->drawWithNamesBestFace();
        }

        if (show_control_wireframe) {
            for (auto v : control_mesh.vertices()) {
                glPushName(v.idx());
                glRasterPos3dv(control_mesh.point(v).data());
                glPopName();
            }
        }

        if (show_offset_lines) {
            for (size_t i = 0, ie = used_construction->getOffsetLines().size(); i < ie; ++i) {
                auto line = used_construction->getOffsetLines().at(i);
                auto p_1 = line->getStart();
                Vec point1(p_1[0], p_1[1], p_1[2]);
                glPushName(i * 2);
                glRasterPos3dv(point1);
                auto p_2 = line->getEnd();
                Vec point2(p_2[0], p_2[1], p_2[2]);
                glPopName();
                glPushName(i * 2 + 1);
                glRasterPos3dv(point2);
                glPopName();
            }
        }
        break;
    }
}

void SmoothingViewer::generateBoundigNet(ConstructionMode curve_mode, size_t resolution) {
    curves = used_construction->generateBoundingCurves(curve_mode);
    for (auto curve : curves)
        for (auto p : curve->generateCurve(resolution))
            curve_points.push_back(p);
}

void SmoothingViewer::drawBoundingNet() const {
    glLineWidth(3.0);
    for (int j = 0; j < curve_points.size() / 30; ++j) {
        glColor3d(1.0, 0.0, 0.0);
        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < 30; i++) {
            const auto& p = curve_points[j * 30 + i];
            glVertex3dv(p.data());
        }
        glEnd();
    }
}

void SmoothingViewer::showBaryC() {
    show_barycenter_points = !show_barycenter_points;
}

void SmoothingViewer::showEdges() {
    show_edges = !show_edges;
}

void SmoothingViewer::showXCps() {
    show_xconstruction_points = !show_xconstruction_points;
}

void SmoothingViewer::showXObj() {
    show_x = !show_x;
}

void SmoothingViewer::showBoundingNet() {
    show_bounding_net = !show_bounding_net;
}

void SmoothingViewer::showVPatches() {
    show_v_patches_controlnet = !show_v_patches_controlnet;
}

void SmoothingViewer::showEPatches() {
    show_e_patches_controlnet = !show_e_patches_controlnet;
}

void SmoothingViewer::showFPatches() {
    show_f_patches_controlnet = !show_f_patches_controlnet;
}

void SmoothingViewer::showBezierStrucure() {
    show_bezier_structure = !show_bezier_structure;
}

void SmoothingViewer::showThreeChordStructure() {
    show_three_chord_structure = !show_three_chord_structure;
}

void SmoothingViewer::showOriginalFace() {
    show_original_face = !show_original_face;
}

void SmoothingViewer::showBestFace() {
    show_best_face = !show_best_face;
}

void SmoothingViewer::showControlWireFrame() {
    show_control_wireframe = !show_control_wireframe;
}

void SmoothingViewer::showOffsetLines() {
    show_offset_lines = !show_offset_lines;
}

void SmoothingViewer::showColoredPatches() {
    show_colored_patches = !show_colored_patches;
}

void SmoothingViewer::showSmoothedMesh() {
    show_smoothed_mesh = !show_smoothed_mesh;
}

bool SmoothingViewer::getBaryCState() {
    return show_barycenter_points;
}

bool SmoothingViewer::getEdgesState() {
    return show_edges;
}

bool SmoothingViewer::getXCpsState() {
    return show_xconstruction_points;
}

bool SmoothingViewer::getXObjState() {
    return show_x;
}

bool SmoothingViewer::getBoundingNetState() {
    return show_bounding_net;
}

bool SmoothingViewer::getVPatchesState() {
    return show_v_patches_controlnet;
}

bool SmoothingViewer::getEPatchesState() {
    return show_e_patches_controlnet;
}

bool SmoothingViewer::getFPatchesState() {
    return show_f_patches_controlnet;
}

bool SmoothingViewer::getBezierStructureState() {
    return show_bezier_structure;
}

bool SmoothingViewer::getThreeChorStructureState() {
    return show_three_chord_structure;
}

bool SmoothingViewer::getOriginalFaceState() {
    return show_original_face;
}

bool SmoothingViewer::getBestFaceState() {
    return show_best_face;
}

bool SmoothingViewer::getControlWireFrameState() {
    return show_control_wireframe;
}

bool SmoothingViewer::getOffsetLinesState() {
    return show_offset_lines;
}

bool SmoothingViewer::getColoredPatchesState() {
    return show_colored_patches;
}