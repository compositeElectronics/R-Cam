# R-Cam
Cross platform CAM - From CAD to Mill

R-Cam is written to take CAD data (in the form of a 3dm file) from your CAD package to a 3-axis milling machine. It is currently under active development which means that there may be bugs and use cases that haven't been tested. It is the USER'S responsibility to ensure that the output from R-Cam is sane and will not damage your machine. If using the output on valuable materials it is strongly advised that a test-cut is run on a cheap material first.

R-Cam uses QT5 and the openNURBS library. QT can be downloaded from https://www.qt.io/ and is usually packaged with Linux distributions. The openNURBS library can be found here: https://www.rhino3d.com/opennurbs

Once compiled, copy the openNURBS library (ending in .a) to the src directory. To build R-Cam change to the "src" directory and type "qmake" and "make" on a command line to build the software.

R-Cam is cross-platform and should build on Linux, Windows or MacOS. It is developed under Linux. No direct support is offered for Windows or MacOS.
