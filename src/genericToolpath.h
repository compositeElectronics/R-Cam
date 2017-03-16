#ifndef GENERICTOOLPATH_H
  #define GENERICTOOLPATH_H

#include <QIODevice>
#include "genericTool.h"
#include "rcamObject.h"
#include "opennurbs_20130711/opennurbs.h"
#include "odPoint.h"
#include "machineSettings.h"
#include "geomReference.h"

class genericToolpath : public rcamObject
{
  Q_OBJECT
  public:
    genericToolpath(machineSettings *settings, const ONX_Model* modelGeom, QString type);
    
    machineSettings *machine;
    genericTool *tool;
    const ONX_Model* geom;
    
    void calculateToolPath();
    void writePath(QIODevice *io);
    
  protected:
    QStringList path;
    
    void createMenu();
    virtual void calcToolPath(const ON_Curve *curve, geomReference* geomRef);
    
  protected slots:
    virtual void addGeometryByLayer();
    void orderGeoms();
};
#endif
