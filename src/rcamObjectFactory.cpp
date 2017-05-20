#include "rcamObjectFactory.h"

#include "geomReference.h"
#include "profilingToolpath.h"
#include "engravingToolpath.h"
#include "surfacingToolpath.h"

rcamObject* rcamObjectFactory::createObject(QDomElement xml, rcamObject *parent){
  rcamObject *object=0;
  
  if (xml.tagName().toLower()=="geometry") object=new geomReference(parent);
  if (xml.tagName().toLower()=="profilingtoolpath") object=new profilingToolpath(parent);
  if (xml.tagName().toLower()=="engravingtoolpath") object=new engravingToolpath(parent);
  if (xml.tagName().toLower()=="surfacingtoolpath") object=new surfacingToolpath(parent);
  
  if (object!=0) object->readXML(xml);
  return object;
}
