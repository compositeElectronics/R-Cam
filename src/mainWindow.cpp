#include "mainWindow.h"
#include <QFileDialog>
#include <QGridLayout>
#include "opennurbs_20130711/opennurbs.h"
#include "genericToolpath.h"
#include "engravingToolpath.h"
#include "profilingToolpath.h"

mainWindow::mainWindow(QMainWindow *parent) : QMainWindow(parent)
{
  QList<int> columns;
  QStringList colNames;
  QList<int> ignores;
  
  ON::Begin();

  fileMenu=menuBar()->addMenu("File");
  openGeometryAction=fileMenu->addAction("Open Geometry");
  connect(openGeometryAction, SIGNAL(triggered()), this, SLOT(openGeometry()));
  saveGCodeAction=fileMenu->addAction("Save G-Code");
  connect(saveGCodeAction, SIGNAL(triggered()), this, SLOT(saveGCode()));
  
  operationsMenu=menuBar()->addMenu("Operations");
  createEngravePathAction=operationsMenu->addAction("Create Engraving Tool Path");
  connect(createEngravePathAction, SIGNAL(triggered()), this, SLOT(engraveObject()));
  createProfilingPathAction=operationsMenu->addAction("Create Profiling Tool Path");
  connect(createProfilingPathAction, SIGNAL(triggered()), this, SLOT(profileObject()));  
  calculateToolPathAction=operationsMenu->addAction("Calculate Tool Path");
  connect(calculateToolPathAction, SIGNAL(triggered()), this, SLOT(calculateToolpaths()));  

  projectTree = new projectTreeDock(this);
  addDockWidget(Qt::LeftDockWidgetArea, projectTree);
  
  objectSettings = new settingsDock(this);
  addDockWidget(Qt::LeftDockWidgetArea, objectSettings);
  
  geomDisplay = new opennurbsDisplayWidget();
  geomDisplay->setGeometry(0,0,1000,800);
  geomDisplay->setModelGeometry(&model);  
  setCentralWidget(geomDisplay);

  machine = new machineSettings();
  machine->addItemToTree(projectTree->treeWidget);
  
  geomDisplay->setObjectSelected(10);
  geomDisplay->setAutoCenterAndZoom(true);
}

void mainWindow::openGeometry(QString fileName){
  bool bModelRead = false;

  if (fileName.isEmpty()){
    fileName = QFileDialog::getOpenFileName(this, "Open Geometry", "", "Rhino Geometry Files (*.3dm)");
    if (fileName.isEmpty()) return;
  }
  
  FILE* fp = ON::OpenFile(fileName.toLatin1().data(), "rb");

  ON_TextLog error_log;

  if (fp!=0){
    ON_BinaryFile archive( ON::read3dm, fp );
    if (model.Read( archive, &error_log)){
     fprintf(stderr,"Geometry read");
    }
    ON::CloseFile( fp );
  }
}

void mainWindow::engraveObject(){
  toolPath.append(new engravingToolpath(machine, &model));
  toolPath.last()->addItemToTree(projectTree->treeWidget);
}

void mainWindow::profileObject(){
  toolPath.append(new profilingToolpath(machine, &model));
  toolPath.last()->addItemToTree(projectTree->treeWidget);
}

void mainWindow::calculateToolpaths(){
  int i;
  for (i=0;i<toolPath.count();i++){
    toolPath[i]->calculateToolPath();
  }
}

void mainWindow::saveGCode(QString fileName){
  int i;
  
  printf("(Initialisation)\n");
  printf("G21 (Set units to mm)\n");
  printf("G90 (Use absolute distances)\n");
  printf("G94 (feed in mm/min)\n");
//  printf("G92.1 (cancel offsets)\n");
  printf("G91.1 (incremental arc mode)\n");
  printf("G54 (use co-ord system 1)\n");
  printf("G98 (set retract behaviour for canned cycles)\n");
  printf("G49 (cancel tool length offset)\n");
  printf("G40 (cancel cutter compensation)\n");
  printf("G17 (select XY plane)\n");
  printf("G80 (cancel canned cycle motion mode)\n");
  printf("\n");
  
  for (i=0;i<toolPath.count();i++) toolPath[i]->printPath();

  printf("M2\n");
}

void mainWindow::treeWidgetContextMenu(QPoint pt){
  QTreeWidgetItem *item=projectTree->treeWidget->itemAt(pt);
  
  rcamObject *bObj = (rcamObject*) item->data(0,Qt::UserRole).value<void*>();
  bObj->menu()->exec(projectTree->treeWidget->mapToGlobal(pt));
}

void mainWindow::treeItemActivated(QTreeWidgetItem *item, int column){
  rcamObject *bObj = (rcamObject*) item->data(0,Qt::UserRole).value<void*>();
  objectSettings->setRcamObject(bObj);
}
