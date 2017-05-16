#ifndef GENERICTOOLPATH_H
  #define GENERICTOOLPATH_H

#include <QIODevice>
#include "genericTool.h"
#include "rcamObject.h"
#include "rcamProject.h"
#include "opennurbs_20130711/opennurbs.h"
#include "odPoint.h"
#include "machineSettings.h"
#include "geomReference.h"

class genericToolpath : public rcamObject
{
  Q_OBJECT
  public:
    genericToolpath(rcamObject *parent, QString type);
    
    machineSettings *machine;
    genericTool *tool;
    const ONX_Model* geom;
    
    void calculateToolPath();
    void writePath(QIODevice *io);
    QList<geomReference*> geometries();
    
  protected:
    QStringList path;
    
    void createMenu();
    virtual void calcToolPath(const ON_Curve *curve, geomReference* geomRef);
    virtual void readXMLElement(QDomElement element);
    virtual void writeCustomXML(QIODevice *xml);
    
  protected slots:
    virtual void addGeometryByLayer();
    void orderGeoms();
};
#endif
