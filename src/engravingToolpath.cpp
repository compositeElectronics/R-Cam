#include "engravingToolpath.h"

engravingToolpath::engravingToolpath(rcamObject *parent) : genericToolpath(parent, "engravingToolpath"){
  setting.append(new editableSetting(QString("depth"), QString("Depth"),     QVariant((double)-0.1), QVariant((double)-75), QVariant((double)75)));
  setting.append(new editableSetting(QString("zStep"), QString("Z Step"),   QVariant((double)-0.5), QVariant(-1.), QVariant(-0.0001)));
  setting.append(new editableSetting(QString("step"), QString("Span Step"),     QVariant((double)0.1), QVariant((double)0.001), QVariant((double)0.25)));
  createSettingsTable();
  createMenu();
}

void engravingToolpath::polishTreeLabel(){
  treeItem->setText(0,QString("Engraving Toolpath"));  
}

void engravingToolpath::calcToolPath(const ON_Curve* curve, geomReference* geomRef){
  // The engrave path is really easy, as you just follow the curve path exactly, moving the cutter down at the start and up at the end.
  // Flatten Z to cut depth to only present XY data.
  double tStart, tEnd;
  double t, tStep, z, tLoop, tRange, zDepth, zStep;
  bool reversed, takeNextCut;
  char line[256];
  ON_3dPoint onPt;
  
  curve->GetDomain(&tStart, &tEnd);
  tRange=tEnd-tStart;
  
  tStep=geomRef->findSettingValue("step").toDouble();
  reversed=geomRef->findSettingValue("reverse").toBool();
  
  zDepth=findSettingValue("depth").toDouble();
  zStep=findSettingValue("zStep").toDouble();
  
  z=0;
  takeNextCut=true;
  while (takeNextCut){
    z+=zStep;
    takeNextCut=false;
    if (z>zDepth){
      takeNextCut=true;
    }else{
      z=zDepth;
    }
    printf("Taking cut at Z=%lf\n",z);
    
    if (!reversed){
      onPt=curve->PointAt(tStart);
    }else{
      onPt=curve->PointAt(tEnd);
    }
    sprintf(line,"G0 X%lf Y%lf",onPt.x, onPt.y); path.append(line);
    sprintf(line,"G0 Z[#<workZ>+#<rapidZ>]\n"); path.append(line);
    sprintf(line,"G1 Z[#<workZ>+%lf]",z); path.append(line);
  
    for (tLoop=0;tLoop<tRange+tStep/10.;tLoop+=tStep){
      if (!reversed){
        t=tStart+tLoop;      
      }else{
        t=tEnd-tLoop;
      }
      onPt=curve->PointAt(t);
      sprintf(line,"G1 X%lf Y%lf",onPt.x, onPt.y);
      path.append(line);
    }
  }
}
