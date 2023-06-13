# About
This repository is about my BSc final thesis project.
A well-known method in 3D geometric modelling is control polyhedron based design, which
allows to create complex free-form objects in an indirect way. The most widespread technique is recursive subdivision where we smooth the polyhedron until it becomes visually
continuous. In the present thesis, the boundary surfaces are produced directly in one step.
The representation is based on the so-called X-construction, by means of witch smooth,
composite surfaces can be produced in different ways. Each method produces standard,
smoothly connected Bézier-surfaces; while providing different types of boundary curves
and geometric continuities.
In my thesis I present the so called three-chord segment structure which can handle curves
with different degrees. I describe the definition of its control points and the necessary algebraic conditions for the smooth connection of adjacent patches (G1 geometric continuity,
common direction blending).
To illustrate the above technique, I have developed a 3D test program that produces a
smoothly connected surface using a given control polyhedron. The user also has the option
of applying three methods and displaying the geometric features of the constructions. In
the last section of the thesis, I also illustrate the results of each algorithm by some test
examples.


# Usage

Pressing `h` displays a help window that contains all the necessary information.
If you want to use this as a base for your own project, examine the source code
and read the documentation of the various libraries (see "Dependencies" below).

# INSTALLATION

Dependencies:

- Qt5 (http://qt-project.org/)
- libQGLViewer (http://www.libqglviewer.com/)
- OpenMesh (http://www.openmesh.org/)

## Linux

    qmake && make

Assumes that:

- qmake (Qt 5.x) is in the path
- QGLViewer include files are in `<standard include directory>/QGLViewer/`
- OpenMesh  include files are in `<standard include directory>/OpenMesh/`
- QGLViewer library file  is  in `<standard library directory>/`
- OpenMesh  library files are in `/usr/lib/OpenMesh/`

If any of the above is not satisfied, edit sample-framework.pro accordingly.

## Windows / Visual Studio

1. Install the Qt SDK, which should integrate itself into Visual Studio.

1. Download the source package for libQGLViewer, and put it somewhere,
   e.g. at c:\Program Files\libQGLViewer. Open Visual Studio,
   in the Qt menu select "Open Qt project file (*.pro)",
   and open the project file in the QGLViewer subdirectory
   (in my case c:\Program Files\libQGLViewer\libQGLViewer.pro).
   Compile a release version of the library.

1. Now open sample-framework.pro, and modify the following line:

        LIBQGLVIEWER_INSTALL_PATH = 'C:\Program Files\libQGLViewer'
   (using the correct path on your system, of course).

1. Download the source package for OpenMesh, and put it somewhere,
   e.g. at c:\Program Files\OpenMesh. Open the solution file
   in Visual Studio and build a release version of the core library.
   Then open sample-framework.pro, and modify the following line:

        OPENMESH_INSTALL_PATH = 'C:\Program Files\OpenMesh'
   (using the correct path on your system, of course).

1. Open Visual Studio, in the Qt menu select "Open Qt project file (*.pro)",
   and open sample-framework.pro.


1. You should be able to build the project, but it won't start. Solution:
   copy QGLViewer2.dll (found in QGLViewer\QGLViewer\release\)
   into c:\Windows\System32 or into the project's directory.

1. If you also want to build a debug version of sample-framework,
   you are still not ready! You have to build debug versions of libQGLViewer
   and OpenMesh first, then change the library names in the project properties
   dialog window (and don't forget to copy QGLViewerd2.dll to a location
   in the path).
