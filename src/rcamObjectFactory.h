#ifndef RCAMOBJECTFACTORY_H
  #define RCAMOBJECTFACTORY_H

  #include <rcamObject.h>

class rcamObjectFactory
{
  public:
    rcamObject* createObject(QDomElement xml, rcamObject *parent=0);
};
#endif
