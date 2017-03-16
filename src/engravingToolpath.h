#ifndef ENGRAVINGTOOLPATH_H
  #define ENGRAVINGTOOLPATH_H
  
#include "genericToolpath.h"
  
class engravingToolpath : public genericToolpath
{
  public:
    engravingToolpath(machineSettings *settings, const ONX_Model* modelGeom);
  
  private:
    void calcToolPath(const ON_Curve* curve, geomReference* geomRef);
    
    void polishTreeLabel();
};
#endif
