#include "genericTool.h"

genericTool::genericTool() : rcamObject(0, "Tool")
{
}
    
odPoint genericTool::toolOffset(odPoint target, odPoint normal){
  return target;  
}
    
