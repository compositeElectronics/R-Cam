#ifndef ENGRAVINGTOOLPATH_H
  #define ENGRAVINGTOOLPATH_H
  
#include "genericToolpath.h"
  
class engravingToolpath : public genericToolpath
{
  public:
    engravingToolpath(rcamObject *parent);
  
  private:
    void calcToolPath(const ON_Curve* curve, geomReference* geomRef);
    
    void polishTreeLabel();
};
#endif
