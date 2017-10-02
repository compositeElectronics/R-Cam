#ifndef GEOMHELPER_H
  #define GEOMHELPER_H

  #include "opennurbs_20130711/opennurbs.h"

class geomHelper
{
  protected:
    bool ptIsInTrimmedAreaOfBREP(ON_Brep *brep, double u, double v, double quality=3);
    double surfaceLengthU(const ON_Surface *srf);
    double surfaceLengthV(const ON_Surface *srf);
};
#endif
