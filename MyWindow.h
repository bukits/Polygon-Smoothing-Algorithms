// -*- mode: c++ -*-
#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets>

#include "SmoothingViewer.h"

class QApplication;
class QProgressBar;

class MyWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MyWindow(QApplication *parent);
  ~MyWindow();

private slots:
	void open();
	void save();
	void setCutoff();
	void setRange();
	void setSlicing();
	void startComputation(QString message);
	void midComputation(int percent);
	void endComputation();
	void quadraticBlending();
	void cubicBlending();
	void smoothing();
	void createDockWindow();
	
	void alphaChanged();
	void showOffsetLines();
	void showBaryPoints();
	void showEdges();
	void showXCps();
	void showXObj();
	void showBoundingNet();
	void showVPatches();
	void showEPatches();
	void showFPatches();
	void showBezierStructure();
	void showThreeChordStructure();
	void showBestFace();
	void showOriginalFace();
	void showWireFrame();

private:
  QApplication *parent;
  SmoothingViewer *viewer;
  QProgressBar *progress;
  QString last_directory;
  QDockWidget* dockWindow;
  QGroupBox* xConstruction;
  QGroupBox* algos;
  QGroupBox* control_structure;
  QGroupBox* control_points;
  QGroupBox* projection;
  QGroupBox* view;
  QDoubleSpinBox* alphaBlending;
};
