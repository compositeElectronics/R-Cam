#ifndef SURFACINGTOOLPATH_H
  #define SURFACINGTOOLPATH_H
  
#include "genericToolpath.h"
#include "geomHelper.h"
  
class surfacingToolpath : public genericToolpath, geomHelper
{
  public:
    surfacingToolpath(rcamObject *parent);
  
  private:
    void calcToolPath(const ON_Brep* brep, geomReference* geomRef, int srfIdx);
    ON_3dPoint calcToolPoint(const ON_Surface *srf, double u, double v, bool flipNormals, double toolRadius);
      
    void polishTreeLabel();
    
    double cutDepth;
    int nCuts;
    
  protected slots:
    void addGeometryByLayer();
};
#endif
