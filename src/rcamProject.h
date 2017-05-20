#ifndef RCAMPROJECT_H
  #define RCAMPROJECT_H

#include <QIODevice>
#include "genericTool.h"
#include "rcamObject.h"
#include "opennurbs_20130711/opennurbs.h"
#include "odPoint.h"
#include "machineSettings.h"
#include "geomReference.h"

class rcamProject : public rcamObject
{
  Q_OBJECT
  public:
    rcamProject(QTreeWidget *parentTree);
    void createMenu();
    
    machineSettings *machine;
    ONX_Model model;
    QString geometryFilename;
    
  public slots:
    void readGeometryFile(QString fileName="");
    void openProject(QString fileName="");
    void saveProject(QString fileName="");
    void saveGCode(QString fileName="");
  
    void engraveObject();
    void profileObject();
    void surfaceObject();
    
    void calculateToolpaths();
    
  private:
    void readXMLElement(QDomElement element);
};
#endif
