#ifndef PROFILINGTOOLPATH_H
  #define PROFILINGTOOLPATH_H
  
#include "genericToolpath.h"
  
class profilingToolpath : public genericToolpath
{
  public:
    profilingToolpath(machineSettings *settings, const ONX_Model* modelGeom);
  
  private:
    void calcToolPath(const ON_Curve* curve);
    
    void polishTreeLabel();
    
    double cutDepth;
    int nCuts;
};
#endif
