#include "genericToolpath.h"
#include "opennurbs_20130711/opennurbs.h"

genericToolpath::genericToolpath(machineSettings *settings, const ONX_Model* modelGeom=0, QString type="toolpath") : rcamObject(0, type)
{
  machine=settings;
  geom=modelGeom;
  setting.append(new editableSetting(QString("feedRate"), QString("Feed Rate"), QVariant((double)700), QVariant(0), QVariant(machine->maxFeed())));
  setting.append(new editableSetting(QString("toolRPM"),  QString("Tool RPM"),  QVariant((double)100), QVariant(0), QVariant(machine->maxCutterRPM())));
}

void genericToolpath::calculateToolPath(){
  int i;
  int objIndex;
  const ON_Curve* curve=0;
  bool match=false;
  
  for (objIndex=0;objIndex<geom->m_object_table.Count();objIndex++){
    const ONX_Model_Object &mo = geom->m_object_table[objIndex];
    match=false;
    for (i=0;i<objectUUID.count();i++){
      if (mo.m_object->ModelObjectId()==objectUUID[i]){
        match=true;
        break;
      }
    }
    if (!match) continue;
      
    switch(mo.m_object->ObjectType()){
      case ON::curve_object:
        curve = ON_Curve::Cast(mo.m_object);
        if (curve) calcToolPath(curve);
        break;
    }
  }
}

void genericToolpath::calcToolPath(const ON_Curve *curve){
}

void genericToolpath::printPath(){
  int i;
  printf("(Set feed and speed)\n");
  printf("F%lf\n", setting[0]->data().toDouble());
  printf("S%lf\n", setting[1]->data().toDouble());
  printf("\n");
  printf("G0 Z%lf\n",machine->safeZ());
  printf("G0 X%lf Y%lf\n",path[0].x, path[0].y);
  printf("G0 Z%lf\n",machine->workpieceZ());
  printf("G1 Z%lf\n", path[0].z);
  for (i=0;i<path.count();i++) printf("G1 X%lf Y%lf Z%lf\n",path[i].x, path[i].y, path[i].z);
  printf("G0 Z%lf\n",machine->safeZ());
}
