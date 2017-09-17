TEMPLATE = app

QT += xml
QT += widgets
#QT += 3dcore 3drender 3dinput 3dlogic 3dextras

DEFINES += TOLERANCE=1e-9

CONFIG -= gui

HEADERS += mainWindow.h \
           odPoint.h \
           odMatrix.h \
           rcamObject.h \
           rcamObjectFactory.h \
           rcamProject.h \
           editableSetting.h \
           geomReference.h \
           genericTool.h \
           engravingTool.h \
           genericToolpath.h \
           engravingToolpath.h \
           profilingToolpath.h \
           surfacingToolpath.h \
           machineSettings.h \
           opennurbsDisplayWidget.h \
           projectTreeDock.h \
           settingsDock.h \
           surfaceCorrection.h

SOURCES += main.cpp \
           mainWindow.cpp \
           odPoint.cpp \
           odMatrix.cpp \
           rcamObject.cpp \
           rcamObjectFactory.cpp \
           rcamProject.cpp \
           editableSetting.cpp \
           geomReference.cpp \
           genericTool.cpp \
           engravingTool.cpp \
           genericToolpath.cpp \
           engravingToolpath.cpp \
           profilingToolpath.cpp \
           surfacingToolpath.cpp \
           machineSettings.cpp \
           opennurbsDisplayWidget.cpp \
           projectTreeDock.cpp \
           settingsDock.cpp \
           surfaceCorrection.cpp
           
LIBS += opennurbs_20130711/libopenNURBS.a

ARCH  = $$system("uname -m")
OPSYS = $$system("uname")
DESTDIR = bin-$$OPSYS-$$ARCH/

UI_DIR = .$$OPSYS-$$ARCH/ui
MOC_DIR = .$$OPSYS-$$ARCH/moc
OBJECTS_DIR = .$$OPSYS-$$ARCH/obj

#!build_pass:message("Generating source for documentation")
#!build_pass:system("doxygen doc.conf > /dev/null")
#!build_pass:message("Building documentation")
#!build_pass:system("make -C doc/latex > /dev/null")
#!build_pass:message("Documentation is available in refman.pdf")
