#ifndef GENERICTOOLPATH_H
  #define GENERICTOOLPATH_H
  
#include "genericTool.h"
#include "rcamObject.h"
#include "opennurbs_20130711/opennurbs.h"
#include "odPoint.h"
#include "machineSettings.h"


class genericToolpath : public rcamObject
{

  public:
    genericToolpath(machineSettings *settings, const ONX_Model* modelGeom, QString type);
    
    machineSettings *machine;
    genericTool *tool;
    const ONX_Model* geom;
    
    void calculateToolPath();
    virtual void printPath();
    
  protected:
    QList<odPoint> path;
    QList<ON_UUID> objectUUID;
    
    virtual void calcToolPath(const ON_Curve *curve);
};
#endif
