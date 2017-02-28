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
           editableSetting.h \
           genericTool.h \
           engravingTool.h \
           genericToolpath.h \
           engravingToolpath.h \
           profilingToolpath.h \
           machineSettings.h \
           opennurbsDisplayWidget.h \
           projectTreeDock.h \
           settingsDock.h

SOURCES += main.cpp \
           mainWindow.cpp \
           odPoint.cpp \
           odMatrix.cpp \
           rcamObject.cpp \
           editableSetting.cpp \
           genericTool.cpp \
           engravingTool.cpp \
           genericToolpath.cpp \
           engravingToolpath.cpp \
           profilingToolpath.cpp \
           machineSettings.cpp \
           opennurbsDisplayWidget.cpp \
           projectTreeDock.cpp \
           settingsDock.cpp
           
LIBS += libopenNURBS-20130711.a

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