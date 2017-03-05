#include "genericToolpath.h"
#include "opennurbs_20130711/opennurbs.h"

#include <QInputDialog>

genericToolpath::genericToolpath(machineSettings *settings, const ONX_Model* modelGeom=0, QString type="toolpath") : rcamObject(0, type)
{
  machine=settings;
  geom=modelGeom;
//  setting.append(new editableSetting(QString("tool"),  QString("Tool"),  QVariant((int)1), QVariant(0), QVariant(99999)));
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
      geometry.append(new geomReference(mo.m_attributes.m_uuid, this));
    }
  }
}

void genericToolpath::calculateToolPath(){
  int i;
  int objIndex;
  const ON_Curve* curve=0;
  bool match=false;
  char line[256];
    
  path.clear();

  path.append("(Set feed and speed)");
  sprintf(line,"F%lf\n", findSettingValue("feedRate").toDouble()); path.append(line);
  sprintf(line,"S%lf\n", findSettingValue("toolRPM").toDouble());  path.append(line);
        
  for (objIndex=0;objIndex<geom->m_object_table.Count();objIndex++){
    const ONX_Model_Object &mo = geom->m_object_table[objIndex];
    for (i=0;i<geometry.count();i++){
      if (mo.m_attributes.m_uuid==geometry[i]->ref && geometry[i]->findSettingValue("enabled").toBool()){
        switch(mo.m_object->ObjectType()){
          case ON::curve_object:
            curve = ON_Curve::Cast(mo.m_object);
            path.append("G0 Z#<safeZ>");
            if (curve) calcToolPath(curve);
            path.append("G0 Z#<safeZ>");
            break;
        }
      }
    }
  }
}

void genericToolpath::calcToolPath(const ON_Curve *curve){
}

void genericToolpath::writePath(QIODevice *io){
  int i;
  if (path.count()==0) return;
  for (i=0;i<path.count();i++){
    io->write(path[i].toLatin1());
    io->write("\n");
  }
}
