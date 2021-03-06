#ifndef GEOMREFERENCE_H
  #define GEOMREFERENCE_H

  #include <QDialog>
  #include <QMenu>
  #include <QMenuBar>
  #include <QAction>
  #include <QDoubleSpinBox>
  #include "rcamObject.h"
  #include "opennurbs_20130711/opennurbs.h"
  
class geomReference : public rcamObject
{
  Q_OBJECT
  public:
    geomReference(ON_UUID uuid, ON::object_type objType, rcamObject* parent=0);
    geomReference(rcamObject* parent=0);
    
    ON_UUID ref;
    ON::object_type type;
    
  private:
    void finishConstruction();
    void polishTreeLabel();
    void createMenu();
    void writeCustomXML(QIODevice *xml);
    void readXMLElement(QDomElement element);
  
  private slots:
    void moveUp();
    void moveDn();
};
#endif
