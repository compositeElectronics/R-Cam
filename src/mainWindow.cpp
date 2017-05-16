#include "mainWindow.h"
#include <QFileDialog>
#include <QFile>
#include <QDomDocument>
#include <QGridLayout>
#include <QMessageBox>
#include "opennurbs_20130711/opennurbs.h"
#include "genericToolpath.h"
#include "engravingToolpath.h"
#include "profilingToolpath.h"
#include "geomReference.h"
  
mainWindow::mainWindow(QMainWindow *parent) : QMainWindow(parent)
{
  QList<int> columns;
  QStringList colNames;
  QList<int> ignores;
  
  ON::Begin();

  projectTree = new projectTreeDock(this);
  addDockWidget(Qt::LeftDockWidgetArea, projectTree);
  
  objectSettings = new settingsDock(this);
  addDockWidget(Qt::LeftDockWidgetArea, objectSettings);

  project = new rcamProject(projectTree->treeWidget);

  geomDisplay = new opennurbsDisplayWidget();
  geomDisplay->setGeometry(0,0,1000,800);
  geomDisplay->setModelGeometry(&project->model);  
  setCentralWidget(geomDisplay);
    
  geomDisplay->setAutoCenterAndZoom(true);
  
  fileMenu=menuBar()->addMenu("File");
  openProjectAction=fileMenu->addAction("Open Project");
  connect(openProjectAction, SIGNAL(triggered()), project, SLOT(openProject()));
  saveProjectAction=fileMenu->addAction("Save Project");
  connect(saveProjectAction, SIGNAL(triggered()), project, SLOT(saveProject()));
}

void mainWindow::treeWidgetContextMenu(QPoint pt){
  QTreeWidgetItem *item=projectTree->treeWidget->itemAt(pt);
  
  rcamObject *bObj = (rcamObject*) item->data(0,Qt::UserRole).value<void*>();
  bObj->menu()->exec(projectTree->treeWidget->mapToGlobal(pt));
}

void mainWindow::treeItemActivated(QTreeWidgetItem *item, int column){
  rcamObject *bObj = (rcamObject*)item->data(0,Qt::UserRole).value<void*>();
  rcamProject *project = dynamic_cast<rcamProject*>(bObj);
  genericToolpath *gentoolpath = dynamic_cast<genericToolpath*>(bObj);
  geomReference *geoRef = dynamic_cast<geomReference*>(bObj);
  
  objectSettings->setRcamObject(bObj);

  if (project){
//    printf("Item matches project\n");
    return;
  }
  
  if (gentoolpath){
//    printf("Item matches (at least) generic tool path\n");
    geomDisplay->setSelectedObjects(gentoolpath->geometries());
    geomDisplay->update();
    return;
  }
  if (geoRef){
//    printf("Item matches geometry reference\n");
    geomDisplay->setSelectedObjects(geoRef);
    geomDisplay->update();
    return;
  }
  geomDisplay->clearSelectedObjects();
}
