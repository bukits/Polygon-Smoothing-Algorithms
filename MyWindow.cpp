#include <memory>
#include "MyWindow.h"

MyWindow::MyWindow(QApplication *parent) :
  QMainWindow(), parent(parent), last_directory(".")
{
  setWindowTitle(tr("Sample 3D Framework"));
  setStatusBar(new QStatusBar);
  progress = new QProgressBar;
  progress->setMinimum(0); progress->setMaximum(100);
  progress->hide();
  statusBar()->addPermanentWidget(progress);

  viewer = new SmoothingViewer(this);
  connect(viewer, SIGNAL(startComputation(QString)), this, SLOT(startComputation(QString)));
  connect(viewer, SIGNAL(midComputation(int)), this, SLOT(midComputation(int)));
  connect(viewer, SIGNAL(endComputation()), this, SLOT(endComputation()));
  setCentralWidget(viewer);

  /////////////////////////
  // Setup actions/menus //
  /////////////////////////

  auto openAction = new QAction(tr("&Open"), this);
  openAction->setShortcut(tr("Ctrl+O"));
  openAction->setStatusTip(tr("Load a model from a file"));
  connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

  auto saveAction = new QAction(tr("&Save as.."), this);
  saveAction->setStatusTip(tr("Save a Bézier surface to a file"));
  connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

  auto quitAction = new QAction(tr("&Quit"), this);
  quitAction->setShortcut(tr("Ctrl+Q"));
  quitAction->setStatusTip(tr("Quit the program"));
  connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

  auto cutoffAction = new QAction(tr("Set &cutoff ratio"), this);
  cutoffAction->setStatusTip(tr("Set mean map cutoff ratio"));
  connect(cutoffAction, SIGNAL(triggered()), this, SLOT(setCutoff()));

  auto rangeAction = new QAction(tr("Set &range"), this);
  rangeAction->setStatusTip(tr("Set mean map range"));
  connect(rangeAction, SIGNAL(triggered()), this, SLOT(setRange()));

  auto slicingAction = new QAction(tr("Set &slicing parameters"), this);
  rangeAction->setStatusTip(tr("Set contouring direction and scaling"));
  connect(slicingAction, SIGNAL(triggered()), this, SLOT(setSlicing()));

  auto showDockWindow = new QAction(tr("Thesis"), this);
  showDockWindow->setStatusTip(tr("Show thesis window"));
  connect(showDockWindow, SIGNAL(triggered()), this, SLOT(createDockWindow()));

  auto fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAction);
  fileMenu->addAction(saveAction);
  fileMenu->addAction(quitAction);

  auto visMenu = menuBar()->addMenu(tr("&Visualization"));
  visMenu->addAction(cutoffAction);
  visMenu->addAction(rangeAction);
  visMenu->addAction(slicingAction);

  dockWindow = new QDockWidget(tr("Controls"), this);
  xConstruction = new QGroupBox(tr("Steps of X Construction"));
  xConstruction->setEnabled(false);
  algos = new QGroupBox(tr("List of smoothing algorithms"));
  algos->setEnabled(false);
  control_points = new QGroupBox(tr("Polieder's patches control points"));
  control_points->setEnabled(false);
  control_structure = new QGroupBox(tr("List of control structures"));
  control_structure->setEnabled(false);
  projection = new QGroupBox(tr("Patch Twist points projection"));
  projection->setEnabled(false);
  view = new QGroupBox(tr("Select view"));
  view->setEnabled(false);

  addDockWidget(Qt::RightDockWidgetArea, dockWindow);
  dockWindow->hide();
  menuBar()->addAction(showDockWindow);
}

void MyWindow::createDockWindow() {
    dockWindow->show();
    QVBoxLayout *vContainer = new QVBoxLayout;
    QVBoxLayout* viewBox = new QVBoxLayout;
    auto* rbSmoothed = new QRadioButton("Show smoothed mesh");
    rbColored = new QRadioButton("Show colored mesh");

    auto cbWireFrame = new QCheckBox("Show wire frame of control mesh");
    cbWireFrame->setChecked(viewer->getControlWireFrameState());
    connect(cbWireFrame, SIGNAL(clicked(bool)), this, SLOT(showWireFrame()));

    connect(rbSmoothed, SIGNAL(clicked(bool)), this, SLOT(showSmoothedMesh()));
    connect(rbColored, SIGNAL(clicked(bool)), this, SLOT(showColoredMesh()));


    auto* text_resolution = new QLabel(tr("Resolution:"));
    resolution = new QSpinBox;
    resolution->setRange(6, 30);
    resolution->setSingleStep(2);
    resolution->setValue(SmoothingViewer::resolution_surface);

    QHBoxLayout* resolution_conatiner = new QHBoxLayout;
    resolution_conatiner->addWidget(text_resolution);
    resolution_conatiner->addWidget(resolution);

    connect(resolution, SIGNAL(valueChanged(int)), this, SLOT(resolutionChanged()));

    viewBox->addWidget(rbSmoothed);
    viewBox->addLayout(resolution_conatiner);
    viewBox->addWidget(rbColored);
    viewBox->addWidget(cbWireFrame);
    view->setLayout(viewBox);
    vContainer->addWidget(view);

    QVBoxLayout* vbox1 = new QVBoxLayout;
    auto* cbBaryC = new QCheckBox("Face's bary center");
    auto* cbEdges = new QCheckBox("New edges");
    auto* cbXCps = new QCheckBox("Points of the X Construction");
    auto* cbXObj = new QCheckBox("X Objects");
    auto* cbCurveNet = new QCheckBox("Bounding curve net");

    cbBaryC->setChecked(viewer->getBaryCState());
    cbEdges->setChecked(viewer->getEdgesState());
    cbXCps->setChecked(viewer->getXCpsState());
    cbXObj->setChecked(viewer->getXObjState());
    cbCurveNet->setChecked(viewer->getBoundingNetState());

    connect(cbBaryC, SIGNAL(clicked(bool)), this, SLOT(showBaryPoints()));
    connect(cbEdges, SIGNAL(clicked(bool)), this, SLOT(showEdges()));
    connect(cbXCps, SIGNAL(clicked(bool)), this, SLOT(showXCps()));
    connect(cbXObj, SIGNAL(clicked(bool)), this, SLOT(showXObj()));
    connect(cbCurveNet, SIGNAL(clicked(bool)), this, SLOT(showBoundingNet()));

    vbox1->addWidget(cbBaryC);
    vbox1->addWidget(cbEdges);
    vbox1->addWidget(cbXCps);
    vbox1->addWidget(cbXObj);
    vbox1->addWidget(cbCurveNet);

    xConstruction->setLayout(vbox1);
    vContainer->addWidget(xConstruction);

    QVBoxLayout* vbox2 = new QVBoxLayout;
    auto* rbQuadratic = new QRadioButton("Quadratic blending");
    auto* rbCubic = new QRadioButton("Cubic blending");
    auto* rbSmoothing = new QRadioButton("Smoothing");

    auto cbOffsetLines = new QCheckBox("Offset lines");
    cbOffsetLines->setChecked(viewer->getOffsetLinesState());
    connect(cbOffsetLines, SIGNAL(clicked(bool)), this, SLOT(showOffsetLines()));

    QHBoxLayout* alpha_conatiner = new QHBoxLayout;

    auto* text_alpha = new QLabel(tr("Alpha:"));

    alphaBlending = new QDoubleSpinBox;
    alphaBlending->setDecimals(3);
    alphaBlending->setRange(0.0, 0.6);
    alphaBlending->setSingleStep(0.1);
    alphaBlending->setValue(0.1);

    alpha_conatiner->addWidget(text_alpha);
    alpha_conatiner->addWidget(alphaBlending);

    connect(alphaBlending, SIGNAL(valueChanged(double)), this, SLOT(alphaChanged()));

    connect(rbQuadratic, SIGNAL(clicked(bool)), this, SLOT(quadraticBlending()));
    connect(rbCubic, SIGNAL(clicked(bool)), this, SLOT(cubicBlending()));
    connect(rbSmoothing, SIGNAL(clicked(bool)), this, SLOT(smoothing()));

    vbox2->addWidget(rbQuadratic);
    vbox2->addWidget(rbCubic);
    vbox2->addWidget(cbOffsetLines);
    vbox2->addLayout(alpha_conatiner);
    vbox2->addWidget(rbSmoothing);
    algos->setLayout(vbox2);
    vContainer->addWidget(algos);

    QVBoxLayout* vbox3 = new QVBoxLayout;
    auto* cbBezierStructure = new QCheckBox("Control structure of Bezier patch");
    auto* cbThreeChordStructure = new QCheckBox("Control Structure of Three-Chord Patch");

    cbBezierStructure->setChecked(viewer->getBezierStructureState());
    cbThreeChordStructure->setChecked(viewer->getThreeChorStructureState());

    connect(cbBezierStructure, SIGNAL(clicked(bool)), this, SLOT(showBezierStructure()));
    connect(cbThreeChordStructure, SIGNAL(clicked(bool)), this, SLOT(showThreeChordStructure()));
    
    vbox3->addWidget(cbBezierStructure);
    vbox3->addWidget(cbThreeChordStructure);
    control_structure->setLayout(vbox3);
    vContainer->addWidget(control_structure);

    QVBoxLayout* vbox4 = new QVBoxLayout;
    auto* cbVPatchesCps = new QCheckBox("Vertex patch's control net");
    auto* cbFPatchesCps = new QCheckBox("Face patch's control net");
    auto* cbEPatchesCps = new QCheckBox("Edge patch's control net");

    cbVPatchesCps->setChecked(viewer->getVPatchesState());
    cbEPatchesCps->setChecked(viewer->getEPatchesState());
    cbFPatchesCps->setChecked(viewer->getFPatchesState());

    connect(cbVPatchesCps, SIGNAL(clicked(bool)), this, SLOT(showVPatches()));
    connect(cbEPatchesCps, SIGNAL(clicked(bool)), this, SLOT(showEPatches()));
    connect(cbFPatchesCps, SIGNAL(clicked(bool)), this, SLOT(showFPatches()));

    vbox4->addWidget(cbVPatchesCps);
    vbox4->addWidget(cbFPatchesCps);
    vbox4->addWidget(cbEPatchesCps);
    control_points->setLayout(vbox4);
    vContainer->addWidget(control_points);

    QVBoxLayout* vbox5 = new QVBoxLayout;
    auto* cbOriginalFace = new QCheckBox("Patch original face");
    auto* cbBestFace = new QCheckBox("Patch face after projection");

    cbOriginalFace->setChecked(viewer->getOriginalFaceState());
    cbBestFace->setChecked(viewer->getBestFaceState());

    connect(cbOriginalFace, SIGNAL(clicked(bool)), this, SLOT(showOriginalFace()));
    connect(cbBestFace, SIGNAL(clicked(bool)), this, SLOT(showBestFace()));

    vbox5->addWidget(cbOriginalFace);
    vbox5->addWidget(cbBestFace);
    projection->setLayout(vbox5);
    vContainer->addWidget(projection);

    QWidget* window = new QWidget;
    window->setLayout(vContainer);
    dockWindow->setWidget(window);
}

MyWindow::~MyWindow() {
}

void MyWindow::open() {
  auto filename =
    QFileDialog::getOpenFileName(this, tr("Open File"), last_directory,
                                 tr("Readable files (*.obj *.ply *.stl *.bzr);;"
                                    "Mesh (*.obj *.ply *.stl);;"
                                    "Bézier surface (*.bzr);;"
                                    "All files (*.*)"));
  if(filename.isEmpty())
    return;
  last_directory = QFileInfo(filename).absolutePath();

  bool ok;
  if (filename.endsWith(".bzr"))
    ok = viewer->openBezier(filename.toUtf8().data());
  else {
      ok = viewer->openMesh(filename.toUtf8().data());
      algos->setEnabled(true);
  }

  if (!ok)
    QMessageBox::warning(this, tr("Cannot open file"),
                         tr("Could not open file: ") + filename + ".");
}

void MyWindow::save() {
  auto filename =
    QFileDialog::getSaveFileName(this, tr("Save File"), last_directory,
                                 tr("Bézier surface (*.bzr);;"));
  if(filename.isEmpty())
    return;
  last_directory = QFileInfo(filename).absolutePath();

  if (!viewer->saveBezier(filename.toUtf8().data()))
    QMessageBox::warning(this, tr("Cannot save file"),
                         tr("Could not save file: ") + filename + ".");
}

void MyWindow::setCutoff() {
  // Memory management options for the dialog:
  // - on the stack (deleted at the end of the function)
  // - on the heap with manual delete or std::unique_ptr 
  // There is also a Qt-controlled automatic deletion by calling
  //     dlg->setAttribute(Qt::WA_DeleteOnClose);
  // ... but this could delete sub-widgets too early.

  auto dlg = std::make_unique<QDialog>(this);
  auto *hb1    = new QHBoxLayout,
       *hb2    = new QHBoxLayout;
  auto *vb     = new QVBoxLayout;
  auto *text   = new QLabel(tr("Cutoff ratio:"));
  auto *sb     = new QDoubleSpinBox;
  auto *cancel = new QPushButton(tr("Cancel"));
  auto *ok     = new QPushButton(tr("Ok"));

  sb->setDecimals(3);
  sb->setRange(0.001, 0.5);
  sb->setSingleStep(0.01);
  sb->setValue(viewer->getCutoffRatio());
  connect(cancel, SIGNAL(pressed()), dlg.get(), SLOT(reject()));
  connect(ok,     SIGNAL(pressed()), dlg.get(), SLOT(accept()));
  ok->setDefault(true);

  hb1->addWidget(text);
  hb1->addWidget(sb);
  hb2->addWidget(cancel);
  hb2->addWidget(ok);
  vb->addLayout(hb1);
  vb->addLayout(hb2);

  dlg->setWindowTitle(tr("Set ratio"));
  dlg->setLayout(vb);

  if(dlg->exec() == QDialog::Accepted) {
    viewer->setCutoffRatio(sb->value());
    viewer->update();
  }
}

void MyWindow::setRange() {
  QDialog dlg(this);
  auto *grid   = new QGridLayout;
  auto *text1  = new QLabel(tr("Min:")),
       *text2  = new QLabel(tr("Max:"));
  auto *sb1    = new QDoubleSpinBox,
       *sb2    = new QDoubleSpinBox;
  auto *cancel = new QPushButton(tr("Cancel"));
  auto *ok     = new QPushButton(tr("Ok"));

  // The range of the spinbox controls the number of displayable digits,
  // so setting it to a large value results in a very wide window.
  double max = 1000.0; // std::numeric_limits<double>::max();
  sb1->setDecimals(5);                 sb2->setDecimals(5);
  sb1->setRange(-max, 0.0);            sb2->setRange(0.0, max);
  sb1->setSingleStep(0.01);            sb2->setSingleStep(0.01);
  sb1->setValue(viewer->getMeanMin()); sb2->setValue(viewer->getMeanMax());
  connect(cancel, SIGNAL(pressed()), &dlg, SLOT(reject()));
  connect(ok,     SIGNAL(pressed()), &dlg, SLOT(accept()));
  ok->setDefault(true);

  grid->addWidget( text1, 1, 1, Qt::AlignRight);
  grid->addWidget(   sb1, 1, 2);
  grid->addWidget( text2, 2, 1, Qt::AlignRight);
  grid->addWidget(   sb2, 2, 2);
  grid->addWidget(cancel, 3, 1);
  grid->addWidget(    ok, 3, 2);

  dlg.setWindowTitle(tr("Set range"));
  dlg.setLayout(grid);

  if(dlg.exec() == QDialog::Accepted) {
    viewer->setMeanMin(sb1->value());
    viewer->setMeanMax(sb2->value());
    viewer->update();
  }
}

void MyWindow::setSlicing() {
  auto dlg = std::make_unique<QDialog>(this);
  auto *hb1    = new QHBoxLayout,
       *hb2    = new QHBoxLayout,
       *hb3    = new QHBoxLayout;
  auto *vb     = new QVBoxLayout;
  auto *text_v = new QLabel(tr("Slicing direction:"));
  QDoubleSpinBox *sb_v[3];
  auto *text_s = new QLabel(tr("Slicing scaling:"));
  auto *sb_s   = new QDoubleSpinBox;
  auto *cancel = new QPushButton(tr("Cancel"));
  auto *ok     = new QPushButton(tr("Ok"));

  for (int i = 0; i < 3; ++i) {
    sb_v[i] = new QDoubleSpinBox;
    sb_v[i]->setDecimals(3);
    sb_v[i]->setRange(-1, 1);
    sb_v[i]->setSingleStep(0.01);
    sb_v[i]->setValue(viewer->getSlicingDir()[i]);
  }
  sb_s->setDecimals(6);
  sb_s->setRange(0, 10000);
  sb_s->setSingleStep(1);
  sb_s->setValue(viewer->getSlicingScaling());
  connect(cancel, SIGNAL(pressed()), dlg.get(), SLOT(reject()));
  connect(ok,     SIGNAL(pressed()), dlg.get(), SLOT(accept()));
  ok->setDefault(true);

  hb1->addWidget(text_v);
  hb1->addWidget(sb_v[0]); hb1->addWidget(sb_v[1]); hb1->addWidget(sb_v[2]);
  hb2->addWidget(text_s);
  hb2->addWidget(sb_s);
  hb3->addWidget(cancel);
  hb3->addWidget(ok);
  vb->addLayout(hb1);
  vb->addLayout(hb2);
  vb->addLayout(hb3);

  dlg->setWindowTitle(tr("Set slicing"));
  dlg->setLayout(vb);

  if(dlg->exec() == QDialog::Accepted) {
    viewer->setSlicingDir(sb_v[0]->value(), sb_v[1]->value(), sb_v[2]->value());
    viewer->setSlicingScaling(sb_s->value());
    viewer->update();
  }
}

void MyWindow::startComputation(QString message) {
  statusBar()->showMessage(message);
  progress->setValue(0);
  progress->show();
  parent->processEvents(QEventLoop::ExcludeUserInputEvents);
}

void MyWindow::midComputation(int percent) {
  progress->setValue(percent);
  parent->processEvents(QEventLoop::ExcludeUserInputEvents);
}

void MyWindow::endComputation() {
  progress->hide();
  statusBar()->clearMessage();
}

void MyWindow::quadraticBlending() {
    viewer->setUpConstruction(SmoothingViewer::ConstructionMode::QUADRATIC, SmoothingViewer::ConstructionMode::QUADRATIC);
    curve_mode = SmoothingViewer::ConstructionMode::QUADRATIC;
    rbColored->setChecked(viewer->getColoredPatchesState());
    control_structure->setEnabled(true);
    projection->setEnabled(true);
    view->setEnabled(true);
    xConstruction->setEnabled(true);
}

void MyWindow::cubicBlending() {
    viewer->setUpConstruction(SmoothingViewer::ConstructionMode::CUBIC, SmoothingViewer::ConstructionMode::CUBIC, alphaBlending->value());
    curve_mode = SmoothingViewer::ConstructionMode::CUBIC;
    rbColored->setChecked(viewer->getColoredPatchesState());
    control_structure->setEnabled(true);
    projection->setEnabled(true);
    view->setEnabled(true);
    xConstruction->setEnabled(true);
}

void MyWindow::smoothing() {
    viewer->setUpConstruction(SmoothingViewer::ConstructionMode::SMOOTHING, SmoothingViewer::ConstructionMode::QUADRATIC);
    curve_mode = SmoothingViewer::ConstructionMode::QUADRATIC;
    rbColored->setChecked(viewer->getColoredPatchesState());
    control_structure->setEnabled(true);
    projection->setEnabled(true);
    view->setEnabled(true);
    xConstruction->setEnabled(true);
}

void MyWindow::showBaryPoints() {
    viewer->showBaryC();
    viewer->update();
}

void MyWindow::showEdges() {
    viewer->showEdges();
    viewer->update();
}

void MyWindow::showXCps() {
    viewer->showXCps();
    viewer->update();
}

void MyWindow::showXObj() {
    viewer->showXObj();
    viewer->update();
}

void MyWindow::showBoundingNet() {
    viewer->showBoundingNet();
    viewer->update();
}

void MyWindow::showVPatches() {
    viewer->showVPatches();
    viewer->update();
}

void MyWindow::showEPatches() {
    viewer->showEPatches();
    viewer->update();
}

void MyWindow::showFPatches() {
    viewer->showFPatches();
    viewer->update();
}

void MyWindow::showBezierStructure() {
    viewer->showBezierStrucure();
    if (!viewer->getBezierStructureState() && !viewer->getThreeChorStructureState())
        control_points->setEnabled(false);
    else control_points->setEnabled(true);
    viewer->update();
}

void MyWindow::showThreeChordStructure() {
    viewer->showThreeChordStructure();
    if (!viewer->getBezierStructureState() && !viewer->getThreeChorStructureState())
        control_points->setEnabled(false);
    else control_points->setEnabled(true);
    viewer->update();
}

void MyWindow::showOriginalFace() {
    viewer->showOriginalFace();
    viewer->update();
}

void MyWindow::showBestFace() {
    viewer->showBestFace();
    viewer->update();
}

void MyWindow::alphaChanged() {
    if (alphaBlending->value() > 0.0f) 
        viewer->setUpConstruction(SmoothingViewer::ConstructionMode::CUBIC, SmoothingViewer::ConstructionMode::CUBIC, alphaBlending->value());
}

void MyWindow::resolutionChanged() {
    if (resolution->value() >= 4) 
        viewer->generateSmoothedMesh(resolution->value());
}

void MyWindow::showOffsetLines() {
    viewer->showOffsetLines();
    viewer->update();
}

void MyWindow::showWireFrame() {
    viewer->showControlWireFrame();
    viewer->update();
}

void MyWindow::showColoredMesh() {
    if (!viewer->getColoredPatchesState()) {
        viewer->showColoredPatches();
        viewer->showSmoothedMesh();
    }
    viewer->update();
}

void MyWindow::showSmoothedMesh() {
    if (!viewer->getSmoothedMeshState()) {
        viewer->showSmoothedMesh();
        viewer->showColoredPatches();
    }
    viewer->update();
}

