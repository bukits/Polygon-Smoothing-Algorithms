#include "BezierSurface.h"

void BezierSurface::generateSurface(size_t resolution) {
    std::vector<MyViewer::MyTraits::Point> tri;

    size_t n = degree[0], m = degree[1];

    std::vector<double> coeff_u, coeff_v;
    for (size_t i = 0; i < resolution; ++i) {
        double u = (double)i / (double)(resolution - 1);
        bernsteinAll(n, u, coeff_u);
        for (size_t j = 0; j < resolution; ++j) {
            double v = (double)j / (double)(resolution - 1);
            bernsteinAll(m, v, coeff_v);
            Vec p(0.0, 0.0, 0.0);
            for (size_t k = 0, index = 0; k <= n; ++k) {
                for (size_t l = 0; l <= m; ++l, ++index) {
                    p += control_points[index] * coeff_u[k] * coeff_v[l];
                }
            }
            handles.push_back(MyViewer::Vector(static_cast<double*>(p)));
        }
    }
    for (size_t i = 0; i < resolution - 1; ++i)
        for (size_t j = 0; j < resolution - 1; ++j) {
            tri.clear();
            tri.push_back(handles[i * resolution + j]);
            tri.push_back(handles[i * resolution + j + 1]);
            tri.push_back(handles[(i + 1) * resolution + j]);
            face_points.push_back(tri);
            
            tri.clear();
            tri.push_back(handles[(i + 1) * resolution + j]);
            tri.push_back(handles[i * resolution + j + 1]);
            tri.push_back(handles[(i + 1) * resolution + j + 1]);
            face_points.push_back(tri);
        }
}

void BezierSurface::colorSurface(Vec color) {
    glColor3d(color.x, color.y, color.z);
    for (auto f_ps : face_points) {
        glBegin(GL_POLYGON);
        for (auto p : f_ps) {
            Vec vec(p[0], p[1], p[2]);
            glVertex3dv(vec);
        }
        glEnd();
    }
}

void BezierSurface::colorSurfaceNames() {
    for (size_t i = 0; i < handles.size(); ++i) {
        auto p = handles.at(i);
        Vec vec(p[0], p[1], p[2]);
        glPushName(i);
        glRasterPos3dv(vec);
        glPopName();
    }
}

size_t* BezierSurface::getDegree() {
	return degree;
}