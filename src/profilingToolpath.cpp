#include "profilingToolpath.h"

profilingToolpath::profilingToolpath(rcamObject *parent) : genericToolpath(parent, "profilingToolpath"){
  setting.append(new editableSetting(QString("zStep"), QString("Z Step"),   QVariant((double)-0.5), QVariant(-100), QVariant(-0.0001)));
  setting.append(new editableSetting(QString("ramp"),  QString("Ramp"),   QVariant((bool)false), QVariant(), QVariant()));
  cutDepth=-13.0;
  nCuts=4;
  createSettingsTable();
  createMenu();
}

void profilingToolpath::addGeometryByLayer(){
  int i;
  genericToolpath::addGeometryByLayer();
  for (i=0;i<children.count();i++){
    children[i]->setting.append(new editableSetting(QString("stepDown"), QString("Step Down"), QVariant((bool)true), QVariant(), QVariant()));
    children[i]->createSettingsTable();
  }
  
}

void profilingToolpath::polishTreeLabel(){
  treeItem->setText(0,QString("Profiling Toolpath"));  
}

void profilingToolpath::calcToolPath(const ON_Curve* curve, geomReference* geomRef){
  // The profiling path is very similar to the engrave path, except that it plunges to the next Z step until it reaches the curve
  // instead of maintaining constant depth.
  
  // If you're feeling brave set the zStep to be greater magnitude than the curve z (i.e. a curve at -1mm could have a step of -5mm)
  // In which case you'll just get the curve profile (and likely crash the CNC machine).
  bool takeNextCut;
  double tStart, tEnd, tRange;
  double t, tStep, tLoop, z, zStep, zi;
  char line[256];
  ON_3dPoint onPt;
  bool reversed, ramp, stepDown;
  
  curve->GetDomain(&tStart, &tEnd);
  tRange=tEnd-tStart;
  
  zStep=findSettingValue("zStep").toDouble();
  ramp=findSettingValue("ramp").toBool();  
  tStep=geomRef->findSettingValue("step").toDouble();
  reversed=geomRef->findSettingValue("reverse").toBool();
  stepDown=geomRef->findSettingValue("stepDown").toBool();
  z=findSettingValue("startZ").toDouble();
  if (!stepDown) z=-1000;
  
  takeNextCut=true;
  if (!reversed){
    onPt=curve->PointAt(tStart);
  }else{
    onPt=curve->PointAt(tEnd);
  }
  
  sprintf(line,"G0 X%lf Y%lf",onPt.x, onPt.y); path.append(line);
  sprintf(line,"G0 Z[#<workZ>+#<rapidZ>]\n"); path.append(line);
  
  if (ramp && stepDown) z=-zStep; // tweak so that ramping will start at z=0.
  
  while (takeNextCut){
    z+=zStep;
    takeNextCut=false;
    printf("Taking cut at Z=%lf\n",z);
    
    if (!curve->IsClosed()){
      tLoop=0;
      if (!reversed){
        t=tStart;
      }else{
        t=tEnd;
      }
      onPt=curve->PointAt(t);
      sprintf(line,"G0 X%lf Y%lf",onPt.x, onPt.y);
      path.append(line);
      
      if (onPt.z>z){
        sprintf(line,"G0 Z[#<workZ>+%lf]", line, onPt.z);
      }else{
        sprintf(line,"G0 Z[#<workZ>+%lf]", line, zi);
      }
      path.append(line);
    }
    
    for (tLoop=0;tLoop<tRange+tStep/10.;tLoop+=tStep){
      if (!reversed){
        t=tStart+tLoop;      
      }else{
        t=tEnd-tLoop;
      }
      if (ramp && stepDown){
        zi=z+tLoop/tRange*zStep;
      }else{
        zi=z;
      }
//      printf("t %lf of %lf to %lf\n",t, tStart, tEnd);
      onPt=curve->PointAt(t);
      sprintf(line,"G1 X%lf Y%lf",onPt.x, onPt.y);
      if (onPt.z>zi){
        sprintf(line,"%s Z[#<workZ>+%lf]", line, onPt.z);
      }else{
        sprintf(line,"%s Z[#<workZ>+%lf]", line, zi);
        takeNextCut=true;
//        printf("Must take next cut\n");
      }
      path.append(line);
    }
    if (!curve->IsClosed()){ sprintf(line,"G0 Z[#<workZ>+#<rapidZ>]\n"); path.append(line); }
  }
}
