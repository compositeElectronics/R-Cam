#ifndef ENGRAVINGTOOLPATH_H
  #define ENGRAVINGTOOLPATH_H
  
#include "genericToolpath.h"
  
class engravingToolpath : public genericToolpath
{
  public:
    engravingToolpath(machineSettings *settings, const ONX_Model* modelGeom);
  
    void printPath();
  
  private:
    void calcToolPath(const ON_Curve* curve);
    
    void polishTreeLabel();
};
#endif
