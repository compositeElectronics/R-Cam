#include "engravingToolpath.h"

engravingToolpath::engravingToolpath(machineSettings *settings, const ONX_Model* modelGeom=0) : genericToolpath(settings, modelGeom, "engravingToolpath"){
  setting.append(new editableSetting(QString("depth"), QString("Depth"),     QVariant((double)-0.1), QVariant((double)-75), QVariant((double)75)));
  setting.append(new editableSetting(QString("step"), QString("Span Step"),     QVariant((double)0.1), QVariant((double)0.001), QVariant((double)0.25)));
  createSettingsTable();
}

void engravingToolpath::polishTreeLabel(){
  treeItem->setText(0,QString("Engraving Toolpath"));  
}

void engravingToolpath::calcToolPath(const ON_Curve* curve){
  // The engrave path is really easy, as you just follow the curve path exactly, moving the cutter down at the start and up at the end.
  // Flatten Z to cut depth to only present XY data.
  double tStart, tEnd;
  double t, tStep, z;
  char line[256];
  ON_3dPoint onPt;
  
  curve->GetDomain(&tStart, &tEnd);
  tStep=findSettingValue("step").toDouble();
  
  z=findSettingValue("depth").toDouble();
  
  onPt=curve->PointAt(tStart);
  sprintf(line,"G0 X%lf Y%lf",onPt.x, onPt.y); path.append(line);
  sprintf(line,"G0 Z[#<workZ>+#<rapidZ>]\n"); path.append(line);
  sprintf(line,"G1 Z[#<workZ>+%lf]",z); path.append(line);
  
  for (t=tStart;t<tEnd+tStep/10.;t+=tStep){
    onPt=curve->PointAt(t);
    sprintf(line,"G1 X%lf Y%lf",onPt.x, onPt.y);
    path.append(line);
  }
}
