#include "genericToolpath.h"
#include "opennurbs_20130711/opennurbs.h"

#include <QInputDialog>

genericToolpath::genericToolpath(rcamObject *parent, QString type="toolpath") : rcamObject(parent, type)
{
  rcamProject *proj=dynamic_cast<rcamProject*>(parent);
  
  if (proj){
    machine=proj->machine;
    geom=&proj->model;
  }
//  setting.append(new editableSetting(QString("tool"),  QString("Tool"),  QVariant((int)1), QVariant(0), QVariant(99999)));
  setting.append(new editableSetting(QString("startZ"),  QString("Start Z"),  QVariant((double)0), QVariant((double)-100), QVariant((double)100)));
  setting.append(new editableSetting(QString("feedRate"), QString("Feed Rate"), QVariant((double)700), QVariant(0), QVariant(machine->maxFeed())));
  setting.append(new editableSetting(QString("toolRPM"),  QString("Tool RPM"),  QVariant((double)100), QVariant(0), QVariant(machine->maxCutterRPM())));
  
}

void genericToolpath::createMenu(){
//  int i;
  QVariant v;
  
  QStringList processNames;
  v.setValue((void *)this);
  treeItem->setData(0, Qt::UserRole, v);
  
  treeItemMenu = new QMenu(p_type);
  objectAction.append(treeItemMenu->addAction("Add geometry by layer"));
  connect(objectAction.last(), SIGNAL(triggered()), this, SLOT(addGeometryByLayer()));
}

void genericToolpath::addGeometryByLayer(){
  int objIndex, i, layId;
  bool ok;
  QStringList layers;
//  ON_Color layerColor;
  
  for (i=0;i<geom->m_layer_table.Count();i++){
//    layerColor=geom->m_layer_table[i].Color();
   //  modelPen.setColor(QColor(objColor.Red(), objColor.Green(), objColor.Blue()));
    layers.append(QString::fromWCharArray(geom->m_layer_table[i].m_name.Array()));
  }
  
  QString item = QInputDialog::getItem(0, "Select geometry by layer", "Layer:", layers, 0, false, &ok);

  if (item.isEmpty()) return;
  
  layId=-1;
  for (i=0;i<geom->m_layer_table.Count();i++){
    if (item==layers[i]){
      layId=i;
      break;
    }
  }
  
  if (layId<0) return;
  
  for (objIndex=0;objIndex<geom->m_object_table.Count();objIndex++){
    const ONX_Model_Object &mo = geom->m_object_table[objIndex];
    if (mo.m_attributes.m_layer_index==layId){
      new geomReference(mo.m_attributes.m_uuid, mo.m_object->ObjectType(), this);
      printf("Child order is currently:\n");
      for (i=0;i<children.count();i++){
        printf("  %i - %s\n", i, children.at(i)->label().toLatin1().data());
      }
    }
  }
}

void genericToolpath::calculateToolPath(){
  int i,j;
  int objIndex;
  int nSrfs;
  const ON_Curve* curve=0;
  const ON_Brep *brep=0;
  bool match=false;
  char line[256];
  geomReference *geometry;
  
  path.clear();

  path.append("(Set feed and speed)");
  sprintf(line,"#<stdFeed>=%lf", findSettingValue("feedRate").toDouble()); path.append(line);
  path.append("F #<stdFeed>");
  sprintf(line,"S%lf\n", findSettingValue("toolRPM").toDouble());  path.append(line);
  
  path.append("G0 Z#<safeZ>");

  for (i=0;i<children.count();i++){
    geometry = dynamic_cast<geomReference*>(children[i]);
//    if (geometry==0) continue;
    
    printf("Calculating %i - %s\n", i, geometry->label().toLatin1().data());

    for (objIndex=0;objIndex<geom->m_object_table.Count();objIndex++){
      const ONX_Model_Object &mo = geom->m_object_table[objIndex];
      
      if (mo.m_attributes.m_uuid==geometry->ref && geometry->findSettingValue("enabled").toBool()){
        switch(mo.m_object->ObjectType()){
          case ON::curve_object:
            curve = ON_Curve::Cast(mo.m_object);
            path.append("G0 Z[#<workZ>+#<rapidZ>]");
            calcToolPath(curve, geometry);
            path.append("G0 Z[#<workZ>+#<rapidZ>]");
            break;
          case ON::brep_object:
            brep=ON_Brep::Cast(mo.m_object);
            nSrfs=brep->m_S.Count();
            for (j=0;j<nSrfs;j++){
              path.append("G0 Z[#<workZ>+#<rapidZ>]");
              calcToolPath(brep, geometry, j);
              path.append("G0 Z[#<workZ>+#<rapidZ>]");
            }
            break;
        }
      }
    }
  }
  path.append("G0 Z#<safeZ>");
}

void genericToolpath::calcToolPath(const ON_Curve *curve, geomReference* geomRef){
}

void genericToolpath::calcToolPath(const ON_Brep* brep, geomReference* geomRef, int srfIdx){
}

void genericToolpath::writeToolPath(QIODevice *io){
  int i;
  if (path.count()==0) return;

  io->write("G0 Z[#<safeZ>]\n");
  for (i=0;i<path.count();i++){
    io->write(path[i].toLatin1());
    io->write("\n");
  }
  io->write("G0 Z[#<safeZ>]\n");
}

void genericToolpath::orderGeoms(){
  
}
  
QList<geomReference*> genericToolpath::geometries(){
  int i;
  QList<geomReference*> ref;
  geomReference *geoRef;
  
  for (i=0;i<children.count();i++){
    geoRef = dynamic_cast<geomReference*>(children[i]);
    if (geoRef) ref.append(geoRef);
  }
  printf("%i geometry references\n",ref.count());
  return ref;
}

void genericToolpath::readXMLElement(QDomElement element){
  if (element.tagName().toLower()=="label") treeItem->setText(0,QString(element.text()));
}

void genericToolpath::writeCustomXML(QIODevice *xml){
  spaceXML(xml,true); xml->write(QString("<label>"+treeItem->text(0)+"</label>\n").toLatin1());
}
