#include "geomReference.h"

geomReference::geomReference(ON_UUID uuid, ON::object_type objType, rcamObject *parent) : rcamObject(parent,"geometry") {
  ref=uuid;
  type=objType;
  finishConstruction();
}

geomReference::geomReference(rcamObject *parent) : rcamObject(parent,"geometry") {
  finishConstruction();
}

void geomReference::finishConstruction(){
  // Some default small machine settings...
  
  setting.append(new editableSetting(QString("reverse"),   QString("Reverse Geometry"), QVariant((bool)false), QVariant(), QVariant()));
  setting.append(new editableSetting(QString("enabled"),   QString("Enabled"),          QVariant((bool)true), QVariant(), QVariant()));
  
  switch(type){
    case ON::curve_object:
      setting.append(new editableSetting(QString("step"),      QString("Span Step"),        QVariant((double)0.1), QVariant((double)0.001), QVariant((double)1)));
      break;
    case ON::brep_object:
      setting.append(new editableSetting(QString("stepOver"),   QString("Step Over distance"),   QVariant((double)0.5), QVariant((double)0.001), QVariant((double)10)));
      setting.append(new editableSetting(QString("stepAlong"),  QString("Span Along distance"),  QVariant((double)0.1), QVariant((double)0.001), QVariant((double)10)));
      setting.append(new editableSetting(QString("flipNormals"),QString("Flip Surface Normals"), QVariant((bool)false), QVariant((bool)false), QVariant((bool)true)));
      setting.append(new editableSetting(QString("domU"),       QString("U direction dominant"), QVariant((bool)true),  QVariant((bool)false), QVariant((bool)true)));
      break;
  }  
  
  createSettingsTable();
  createMenu();
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

void geomReference::writeCustomXML(QIODevice *xml){
  char *mref;
  mref = new char[38];
  mref = ON_UuidToString( ref , mref);
  
  spaceXML(xml,true); xml->write(QString("<label>"+treeItem->text(0)+"</label>\n").toLatin1());
  spaceXML(xml,true); xml->write(QString("<uuid>"+QString(mref)+"</uuid>\n").toLatin1());
}

void geomReference::readXMLElement(QDomElement element){
  if (element.tagName().toLower()=="label") treeItem->setText(0,QString(element.text()));
  if (element.tagName().toLower()=="uuid") ref=ON_UuidFromString(element.text().toLatin1().data());
}
