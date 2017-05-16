#ifndef PROFILINGTOOLPATH_H
  #define PROFILINGTOOLPATH_H
  
#include "genericToolpath.h"
  
class profilingToolpath : public genericToolpath
{
  public:
    profilingToolpath(rcamObject *parent);
  
  private:
    void calcToolPath(const ON_Curve* curve, geomReference* geomRef);
    
    void polishTreeLabel();
    
    double cutDepth;
    int nCuts;
    
  protected slots:
    void addGeometryByLayer();
};
#endif
