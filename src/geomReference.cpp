#include "geomReference.h"

geomReference::geomReference(ON_UUID uuid, rcamObject *parent) : rcamObject(parent,"geometry") {
  // Some default small machine settings...
  
  setting.append(new editableSetting(QString("reverse"),   QString("Reverse Geometry"), QVariant((bool)false), QVariant(), QVariant()));
  setting.append(new editableSetting(QString("enabled"),   QString("Enabled"),          QVariant((bool)true), QVariant(), QVariant()));
  
  createSettingsTable();
  createMenu();
  ref=uuid;
}

void geomReference::polishTreeLabel(){
  treeItem->setFlags(treeItem->flags() & ~Qt::ItemIsEditable);
  treeItem->setText(0,QString("Geometry"));
}

void geomReference::createMenu(){
//  int i;
  QVariant v;
  
  QStringList processNames;
  v.setValue((void *)this);
  treeItem->setData(0, Qt::UserRole, v);
  
  treeItemMenu = new QMenu(p_type);
  objectAction.append(treeItemMenu->addAction("Move up"));
  connect(objectAction.last(), SIGNAL(triggered()), this, SLOT(moveUp()));
  objectAction.append(treeItemMenu->addAction("Move down"));
  connect(objectAction.last(), SIGNAL(triggered()), this, SLOT(moveDn()));
}

void geomReference::moveUp(){
  int from;
  // Find child...
  from=p_parent->childIndex(this);
  printf("Child Index = %i\n",from);
  p_parent->moveChild(from, from-1);
}

void geomReference::moveDn(){
  int from;
  // Find child...
  from=p_parent->childIndex(this);
  printf("Child Index = %i\n",from);
  p_parent->moveChild(from, from+1);
}
