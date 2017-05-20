#ifndef SURFACINGTOOLPATH_H
  #define SURFACINGTOOLPATH_H
  
#include "genericToolpath.h"
  
class surfacingToolpath : public genericToolpath
{
  public:
    surfacingToolpath(rcamObject *parent);
  
  private:
    void calcToolPath(const ON_Surface* srf, geomReference* geomRef);
    
    void polishTreeLabel();
    
    double cutDepth;
    int nCuts;
    
  protected slots:
    void addGeometryByLayer();
};
#endif
