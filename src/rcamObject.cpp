#include "rcamObject.h"
#include <stdio.h>

#include "rcamObjectFactory.h"

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
//  if (p_settingsTable) delete p_settingsTable;
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

QString rcamObject::label(){
  if (treeItem!=0) return treeItem->text(0);
  return QString("No label");
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
  
  for (i=0;i<setting.count();i++){
    setting.at(i)->writeXML(xml,hlevel+2);
  }
  
  writeCustomXML(xml);
  
  for (i=0;i<children.count();i++){
    children.at(i)->writeXML(xml);
  }
  
  spaceXML(xml,false); sprintf(line,"</%s>\n",p_type.toLatin1().data()); xml->write(line);
}

void rcamObject::writeCustomXML(QIODevice *xml){
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
  bool found;
  int i;
  QDomElement element = root.firstChildElement();
  rcamObjectFactory factory;
  rcamObject *tmpObject;
  
  printf("root tag = %s\n",root.tagName().toLatin1().data());
  while (!element.isNull()) {
    printf("Element tag = %s\n",element.tagName().toLatin1().data());
    if (element.tagName().toLower()=="setting"){
      printf("Setting Name = %s\n", element.attribute("name").toLatin1().data());
      for (i=0;i<setting.count();i++){
        setting.at(i)->readXMLElement(element);
      }
      printf("Checking for non-default setting...\n");
      // check for non-default setting... (some toolpaths extend the available settings.
      found=false;
      for (i=0;i<setting.count();i++){
        if (setting.at(i)->isApplicable(element)) found=true;
      }
      if (found==false){
        printf("Adding new setting to list\n");
        setting.append(new editableSetting(element));
      }
    }
    tmpObject=factory.createObject(element,this);

    readXMLElement(element);
    element = element.nextSiblingElement();
  }
  createSettingsTable();
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

int rcamObject::childIndex(rcamObject *query){
  int i;
  for (i=0;i<children.count();i++){
    if (children.at(i)==query) return i;
  }
  return -1;
}
  
void rcamObject::moveChild(int from, int to){
  int i;
  if (from>=children.count() || from<0) return;
  if (to>=children.count() || to<0) return;
  
  printf("Child order is currently:\n");
  for (i=0;i<children.count();i++){
    printf("  %i - %s\n", i, children.at(i)->label().toLatin1().data());
  }
  
  printf("Move child from %i to %i\n", from, to);
  
  children.move(from,to);
  
  printf("Child order is now:\n");
  for (i=0;i<children.count();i++){
    printf("  %i - %s\n", i, children.at(i)->label().toLatin1().data());
  }
  
  QTreeWidgetItem* child = treeItem->takeChild(from);
  treeItem->insertChild(to, child);
}
