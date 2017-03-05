#include "geomReference.h"

geomReference::geomReference(ON_UUID uuid, rcamObject *parent) : rcamObject(parent,"geometry") {
  // Some default small machine settings...
  
  setting.append(new editableSetting(QString("reverse"),   QString("Reverse Geometry"), QVariant((bool)false), QVariant(), QVariant()));
  setting.append(new editableSetting(QString("enabled"),   QString("Enabled"),          QVariant((bool)true), QVariant(), QVariant()));
  
  createSettingsTable();
  ref=uuid;
}

void geomReference::polishTreeLabel(){
  treeItem->setFlags(treeItem->flags() & ~Qt::ItemIsEditable);
  treeItem->setText(0,QString("Geometry"));
}
