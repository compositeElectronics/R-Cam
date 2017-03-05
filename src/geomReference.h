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
    geomReference(ON_UUID uuid, rcamObject* parent=0);
    
    ON_UUID ref;
    
  private:
    void polishTreeLabel();
};
#endif
