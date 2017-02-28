#ifndef GENERICTOOL_H
  #define GENERICTOOL_H

#include "odPoint.h"
#include "rcamObject.h"
  
class genericTool : public rcamObject
{

  public:
    genericTool();
    
    odPoint toolOffset(odPoint target, odPoint normal);
    
  protected:
    QStringList setting;
    QList<double> value;
};
#endif
