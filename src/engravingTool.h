#ifndef ENGRAVINGTOOL_H
  #define ENGRAVINGTOOL_H

#include "genericTool.h"
  
class engravingTool : public genericTool
{

  public:
    engravingTool();
    
    odPoint toolOffset(odPoint target, odPoint normal);
};
#endif
