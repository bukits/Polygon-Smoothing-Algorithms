// -*- mode: c++ -*-
#pragma once

#include <string>

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <QGLViewer/qglviewer.h>

class BezierSurface;

using qglviewer::Vec;

class MyViewer : public QGLViewer {
  Q_OBJECT

public:
  explicit MyViewer(QWidget *parent);
  virtual ~MyViewer();

  inline double getCutoffRatio() const;
  inline void setCutoffRatio(double ratio);
  inline double getMeanMin() const;
  inline void setMeanMin(double min);
  inline double getMeanMax() const;
  inline void setMeanMax(double max);
  inline const double *getSlicingDir() const;
  inline void setSlicingDir(double x, double y, double z);
  inline double getSlicingScaling() const;
  inline void setSlicingScaling(double scaling);
  bool openBezier(const std::string &filename, bool update_view = true);
  bool saveBezier(const std::string &filename);

  struct MyTraits : public OpenMesh::DefaultTraits {
      using Point = OpenMesh::Vec3d; // the default would be Vec3f
      using Normal = OpenMesh::Vec3d;
      VertexTraits{
        double mean;              // approximated mean curvature
      };
  };

  using PolyMesh = OpenMesh::PolyMesh_ArrayKernelT<MyTraits>;
  using Vector = OpenMesh::VectorT<double, 3>;

signals:
  void startComputation(QString message);
  void midComputation(int percent);
  void endComputation();

protected:
  virtual bool openMesh(const std::string& filename, bool update_view = true);
  virtual void init() override;
  virtual void draw() override;
  virtual void drawWithNames() override;
  virtual void postSelection(const QPoint &p) override;
  virtual void keyPressEvent(QKeyEvent *e) override;
  virtual void mouseMoveEvent(QMouseEvent *e) override;
  virtual QString helpString() const override;
  void updateMeanCurvature(bool update_min_max = true);

  // Mesh
  virtual void updateMesh(bool update_mean_range = true);
  void updateVertexNormals();

  PolyMesh mesh;

  enum class ModelType { NONE, MESH, BEZIER_SURFACE } model_type;
  bool show_colored_patches;

private:
  using TriMesh = OpenMesh::TriMesh_ArrayKernelT<MyTraits>;

#ifdef USE_JET_NORMALS
  void updateVertexNormalsWithJetFit(size_t neighbors);
#endif
  void localSystem(const Vector &normal, Vector &u, Vector &v);
  double voronoiWeight(TriMesh::HalfedgeHandle in_he);
  void updateMeanMinMax();

  // Visualization
  void setupCamera();
  Vec meanMapColor(double d) const;
  void drawControlNet() const;
  void drawAxes() const;
  void drawAxesWithNames() const;
  static Vec intersectLines(const Vec &ap, const Vec &ad, const Vec &bp, const Vec &bd);

  // Other
  void fairMesh();

  //////////////////////
  // Member variables //
  //////////////////////

  // Mesh
  TriMesh tri_mesh;

  // Bezier
  BezierSurface* opened_bezier;

  // Visualization
  double mean_min, mean_max, cutoff_ratio;
  bool show_control_points, show_solid, show_wireframe;
  GLuint isophote_texture, environment_texture, current_isophote_texture, slicing_texture;
  Vector slicing_dir;
  enum class Visualization { PLAIN, MEAN, SLICING, ISOPHOTES } visualization;
  double slicing_scaling;
  int selected_vertex;
  struct ModificationAxes {
    bool shown;
    float size;
    int selected_axis;
    Vec position, grabbed_pos, original_pos;
  } axes;
  std::string last_filename;
};

#include "MyViewer.hpp"
