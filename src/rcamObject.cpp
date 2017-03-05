#include "rcamObject.h"
#include <stdio.h>

rcamObject::rcamObject(rcamObject* parent, QString processType) : QObject(){
  p_parent=parent;
  p_type=processType;
  hlevel=0;
  
  if (parent!=0){
    // Add object to parent object's children list
    p_parent->children.append(this);
    hlevel=p_parent->hlevel+1;
    // Add object to the tree view
    addItemToTree(p_parent->treeItem);
  }
//  printf("hLevel = %i\n",hlevel);
}

void rcamObject::addItemToTree(QTreeWidget *parentTree){
  treeItem = new QTreeWidgetItem(QStringList(p_type));
  treeItem->setFlags(treeItem->flags() | Qt::ItemIsEditable);
  parentTree->insertTopLevelItem(0, treeItem);
  createMenu();
  polishTreeLabel();
}
  
void rcamObject::addItemToTree(QTreeWidgetItem *parentTreeItem){
  treeItem = new QTreeWidgetItem(parentTreeItem, QStringList(p_type));
  treeItem->setFlags(treeItem->flags() | Qt::ItemIsEditable);
  createMenu();
  polishTreeLabel();
}

void rcamObject::polishTreeLabel(){
}

void rcamObject::createMenu(){
//  int i;
  QVariant v;
  
  QStringList processNames;
  v.setValue((void *)this);
  treeItem->setData(0, Qt::UserRole, v);
  
  treeItemMenu = new QMenu(p_type);
//  objectAction.append(treeItemMenu->addAction("Example"));
}

QWidget* rcamObject::settingsTable(){
  return p_settingsTable;
}

void rcamObject::createSettingsTable(){
  int i;
  p_settingsTable = new  QTableWidget(setting.count(),2);
  
  for (i=0;i<setting.count();i++){
    p_settingsTable->setCellWidget(i,0,setting[i]->label());
    p_settingsTable->setCellWidget(i,1,setting[i]->editor());
  }                                   
}

QMenu* rcamObject::menu(){
  return treeItemMenu;
}

QString rcamObject::type(){
  return p_type;
}

void rcamObject::executeTree(){
  int i;
  execute();
  for (i=0;i<children.count();i++){
    children.at(i)->executeTree();
  }
}

void rcamObject::execute(){
  // Dummy function re-implement in your own code.
}

void rcamObject::writeXML(QIODevice *xml){
  int i;
  char line[4096];

  spaceXML(xml,false); sprintf(line,"<%s>\n",p_type.toLatin1().data()); xml->write(line);
  
  writeSettingsXML(xml);
  
  for (i=0;i<children.count();i++){
    children.at(i)->writeXML(xml);
  }
  
  spaceXML(xml,false); sprintf(line,"</%s>\n",p_type.toLatin1().data()); xml->write(line);
}

void rcamObject::writeSettingsXML(QIODevice *xml){
}

void rcamObject::spaceXML(QIODevice *xml, bool inside){
  int n;
  n=hlevel+1;
  if (inside) n++;
  QByteArray ba=QByteArray("  ").repeated(n);
//  printf("Spacing= \"%s\"\n",ba.data());
  xml->write(ba); 
}

void rcamObject::readXML(QDomElement root){
  QDomElement element = root.firstChildElement();
//  printf("root tag = %s\n",root.tagName().toLatin1().data());
  while (!element.isNull()) {
//    printf("element tag = %s\n",element.tagName().toLatin1().data());
    readXMLElement(element);
    element = element.nextSiblingElement();
  }
}

void rcamObject::readXMLElement(QDomElement element){
}

QVariant rcamObject::findSettingValue(QString name){
  int i;
  for (i=0;i<setting.count();i++){
    if (setting[i]->settingName()==name) return setting[i]->data();
  }
  
  return QVariant();
}
