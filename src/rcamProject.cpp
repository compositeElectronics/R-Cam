#include "rcamProject.h"
#include "genericToolpath.h"
#include "engravingToolpath.h"
#include "profilingToolpath.h"
#include "surfacingToolpath.h"
#include "geomReference.h"
#include "opennurbs_20130711/opennurbs.h"
#include <QFileDialog>
#include <QFile>
#include <QDomDocument>
#include <QInputDialog>
#include <QMessageBox>

rcamProject::rcamProject(QTreeWidget *parentTree) : rcamObject(0, "project")
{
  printf("Creating new project\n");
  addItemToTree(parentTree);
  machine = new machineSettings(this);
  createSettingsTable();
  createMenu();
  printf("Project created\n");
}

void rcamProject::createMenu(){
  QVariant v;
  
  QStringList processNames;
  v.setValue((void *)this);
  treeItem->setData(0, Qt::UserRole, v);
  
  treeItemMenu = new QMenu(p_type);
  objectAction.append(treeItemMenu->addAction("Open Geometry"));
  connect(objectAction.last(), SIGNAL(triggered()), this, SLOT(readGeometryFile()));
  objectAction.append(treeItemMenu->addAction("Save G-Code"));
  connect(objectAction.last(), SIGNAL(triggered()), this, SLOT(saveGCode()));

  treeItemMenu->addSeparator();
  
  objectAction.append(treeItemMenu->addAction("Create Engraving Tool Path"));
  connect(objectAction.last(), SIGNAL(triggered()), this, SLOT(engraveObject()));

  objectAction.append(treeItemMenu->addAction("Create Profiling Tool Path"));
  connect(objectAction.last(), SIGNAL(triggered()), this, SLOT(profileObject()));
  
  objectAction.append(treeItemMenu->addAction("Create Surfacing Tool Path"));
  connect(objectAction.last(), SIGNAL(triggered()), this, SLOT(surfaceObject()));
  
  objectAction.append(treeItemMenu->addAction("Calculate Tool Path"));
  connect(objectAction.last(), SIGNAL(triggered()), this, SLOT(calculateToolpaths()));
}

void rcamProject::readGeometryFile(QString fileName){
  bool bModelRead = false;
  int objIndex;
  
  if (fileName.isEmpty()){
    fileName = QFileDialog::getOpenFileName(0, "Open Geometry", "", "Rhino Geometry Files (*.3dm)");
    if (fileName.isEmpty()) return;
  }
  
  FILE* fp = ON::OpenFile(fileName.toLatin1().data(), "rb");

  ON_TextLog error_log;

  if (fp!=0){
    ON_BinaryFile archive( ON::read3dm, fp );
    if (model.Read( archive, &error_log)){
     fprintf(stderr,"Geometry read\n");
     geometryFilename=fileName;
    }
    ON::CloseFile( fp );
  }
  
  int pt, ptset, crv, srf, brep, mesh, extrusion;
  
  pt=0; ptset=0; crv=0; srf=0; brep=0; mesh=0; extrusion=0;

  for (objIndex=0;objIndex<model.m_object_table.Count();objIndex++){
    const ONX_Model_Object &mo = model.m_object_table[objIndex];
        
    switch( mo.m_object->ObjectType() ){
      case ON::point_object:
        pt++;
        break;
      case ON::pointset_object:
        ptset++;
        break;
      case ON::curve_object:
        crv++;
        break;
      case ON::surface_object:
        srf++;
        break;
      case ON::brep_object:
        brep++;
        break;
      case ON::extrusion_object:
        extrusion++;
        break;
      case ON::mesh_object:
        mesh++;
        break;
    };
  }
  
  printf("Rhino model contains:\n");
  printf("      Point: %i\n",pt);
  printf("  Point set: %i\n",ptset);
  printf("      Curve: %i\n",crv);
  printf("    Surface: %i\n",srf);
  printf("      B-Rep: %i\n",brep);
  printf("       Mesh: %i\n",mesh);
}

void rcamProject::saveGCode(QString fileName){
  int i;
  char line[256];
  
  if (fileName.isEmpty()) fileName=QFileDialog::getSaveFileName(0, tr("Save GCode File"), QDir::currentPath(), tr("G-Code Files (*.ngc)"));
  if (fileName.isEmpty()) return;

  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(0, "Filter", QString("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString()));
    return;
  }
  
  file.write("(Constants)\n");
  sprintf(line,"#<safeZ>=%lf\n",machine->findSettingValue("safeZ").toDouble()); file.write(line);
  sprintf(line,"#<workZ>=%lf\n",machine->findSettingValue("workZ").toDouble()); file.write(line);
  sprintf(line,"#<rapidZ>=%lf\n",machine->findSettingValue("rapidZ").toDouble()); file.write(line);
  
  file.write("(Initialisation)\n");
  file.write("G21 (Set units to mm)\n");
  file.write("G90 (Use absolute distances)\n");
  file.write("G94 (feed in mm/min)\n");
  file.write("G91.1 (incremental arc mode)\n");
  file.write("G54 (use co-ord system 1)\n");
  file.write("G98 (set retract behaviour for canned cycles)\n");
  file.write("G49 (cancel tool length offset)\n");
  file.write("G40 (cancel cutter compensation)\n");
  file.write("G17 (select XY plane)\n");
  file.write("G80 (cancel canned cycle motion mode)\n");
  file.write("\n");
  
  for (i=0;i<children.count();i++) children[i]->writeToolPath(&file);
  
  file.write("M2\n");
}

void rcamProject::openProject(QString fileName){
  QString errorStr;
  int errorLine, errorColumn;
  QDomDocument domDocument;
  
  if (fileName.isEmpty()){
    fileName = QFileDialog::getOpenFileName(0, "Open Project", "", "R-CAM project files (*.xml)");
    if (fileName.isEmpty()) return;
  }
  
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(0, tr("R-CAM"), tr("Cannot read file %1\n%2.").arg(fileName).arg(file.errorString()));
    return;
  }

  if (!domDocument.setContent(file.readAll(), true, &errorStr, &errorLine, &errorColumn)){
    QMessageBox::information(0, tr("R-CAM"), tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));
    return;
  }
  file.close();
  
  QDomElement root = domDocument.documentElement();
  readXML(root);
}

void rcamProject::readXMLElement(QDomElement element){
  if (element.tagName().toLower()=="geometryfile"){
    if (element.attribute("type").toLower()=="3dm") readGeometryFile(element.text());
  }
}

void rcamProject::saveProject(QString fileName){
  int i;
  char line[256];
  
  if (fileName.isEmpty()) fileName=QFileDialog::getSaveFileName(0, tr("Save Project File"), QDir::currentPath(), tr("R-Cam Project File (*.xml)"));
  if (fileName.isEmpty()) return;

  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(0, "Filter", QString("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString()));
    return;
  }
  
  file.write("<rcam>\n");
  file.write(QString("  <geometryFile type=\"3dm\">"+geometryFilename+"</geometryFile>\n").toLatin1());
    
  for (i=0;i<children.count();i++) children[i]->writeXML(&file);

  file.write("</rcam>\n");
}

void rcamProject::engraveObject(){
  genericToolpath* toolPath;
  toolPath = new engravingToolpath(this);
//  toolPath->createMenu();
}

void rcamProject::profileObject(){
  genericToolpath* toolPath;
  toolPath = new profilingToolpath(this);
//  toolPath->createMenu();
}

void rcamProject::surfaceObject(){
  genericToolpath* toolPath;
  toolPath = new surfacingToolpath(this);
//  toolPath->createMenu();
}

void rcamProject::calculateToolpaths(){
  int i;
  genericToolpath *toolpath;
  
  for (i=0;i<children.count();i++){
    toolpath = dynamic_cast<genericToolpath*>(children[i]);
    if (toolpath){
      toolpath->calculateToolPath();
    }
  }
}
